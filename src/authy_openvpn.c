#ifdef WIN32
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#define AUTHY_VPN_CONF "authy-vpn.conf"
#pragma comment(lib, "ws2_32.lib")

#else
#define AUTHY_VPN_CONF "/etc/openvpn/authy/authy-vpn.conf"
#endif

#include <stdarg.h>

#include "authy-conf.h"
#include "openvpn-plugin.h"
#include "jsmn.h"
#include "authy_api.h"

static int debug(const int verb, const int line, const char *format, ...)
{
	if(verb >= 4)
	{
		printf("Authy Plugin: %d\t", line);
		va_list arg;
		int done;

    va_start(arg, format);
    done = vfprintf (stderr, format, arg);
    va_end(arg);
    fflush(stderr);
    return done;
	}
	return 0;
}

/*
 * This state expects the following config line
 * plugin authy-openvpn.so APIURL APIKEY PAM
 * where APIURL should be something like  https://api.authy.com/protected/json
 * APIKEY like d57d919d11e6b221c9bf6f7c882028f9
 * PAM pam | nopam # it is nopam by default
 * pam = 1;
 * nopam = 0;
 */
struct plugin_context {
  char *psz_API_url;
  char *psz_API_key;
  int b_PAM;
  int verb;
};

/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 * From openvpn/sample/sample-plugins/defer/simple.c
 */
static char *
get_env(const char *name, const char *envp[])
{
  if (envp)
  {
    int i;
    const int namelen = strlen (name);
    for (i = 0; envp[i]; ++i)
    {
      if (!strncmp (envp[i], name, namelen))
      {
        const char *cp = envp[i] + namelen;
        if (*cp == '=')
          return (char *) cp + 1;
      }
    }
  }
  return NULL;
}

/*
 * Plugin initialization
 * it registers the functions that we want to intercept (OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY)
 * and initializes the plugin context that holds the api url, api key and if we are using PAM
 */
OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1(unsigned int *type_mask, const char *argv[], const char *envp[])
{
  /* Context Allocation */
  struct plugin_context *context;

  context = (struct plugin_context *) calloc(1, sizeof(struct plugin_context));
  
  if(!context){
    goto error;
  }
  /* Save the verbosite level from env */
  const char *verb_string = get_env ("verb", envp);
  if (verb_string){
	context->verb = atoi (verb_string);
  }

  if(argv[1] && argv[2])
  {
	debug(context->verb, __LINE__, "Plugin path = %s\n", argv[0]);
	debug(context->verb, __LINE__, "api url = %s, api key %s\n", argv[1], argv[2]); 
    context->psz_API_url = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
    strncpy(context->psz_API_url, argv[1], strlen(argv[1]));

    context->psz_API_key = (char *) calloc(strlen(argv[2]) + 1, sizeof(char));
    strncpy(context->psz_API_key, argv[2], strlen(argv[2]));

    context->b_PAM  = 0;
	
  }

  if (argv[3] && strncmp(argv[3], "pam", 3) == SUCCESS)
    context->b_PAM = 1;

  /* Set type_mask, a.k.a callbacks that we want to intercept */
  *type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);

  /* Cast and return the context */
  return (openvpn_plugin_handle_t) context;

error:
  return (openvpn_plugin_handle_t)FAILURE;
}

/*
 * parse response
 * it tokenizes the json response from the api, and traverse looking for the 'success' node
 */
static int
parse_response(char *psz_response)
{
  int cnt;
  jsmn_parser parser;
  jsmn_init(&parser);
  jsmntok_t tokens[20];
  jsmn_parse(&parser, psz_response, tokens, 20);

  /* success isn't always on the same place, look until 19 because it
     shouldn't be the last one because it won't be a key */
  for (cnt = 0; cnt < 19; ++cnt)
  {
    if(strncmp(psz_response + (tokens[cnt]).start, "success", (tokens[cnt]).end - (tokens[cnt]).start) == 0)
    {
      if(strncmp(psz_response + (tokens[cnt+1]).start, "true", (tokens[cnt+1]).end - (tokens[cnt+1]).start) == 0){
        return SUCCESS;
      } else {
        return FAILURE;
      }
    }
  }
  return FAILURE;
}

/*
 * authenticate
 * this is real core of the plugin
 * it handles the authentication agains Authy services
 * using the password field to obtain the OTP
 * and as other authentication plugins it sets its authenication status
 * to the control file
 */
static int
authenticate(struct plugin_context *context, const char *argv[], const char *envp[])
{
  int i_status;
  char *psz_token, *psz_control, *psz_response, *psz_common_name, *psz_username;
  char  psz_authy_ID[20];
  FILE *p_file_auth;

  i_status = FAILURE;

  psz_common_name = get_env("common_name", envp);
  psz_username    = get_env("username", envp);
  psz_token       = get_env("password", envp);
  psz_control     = get_env("auth_control_file", envp);
  psz_response    = (char *) calloc(255, sizeof(char));

  debug(context->verb, __LINE__, "[Authy] Authy Two-Factor Authentication started\n");
  debug(context->verb, __LINE__, "[Authy] Authenticating:  ");

  if(!psz_common_name || !psz_token || !psz_username || !psz_response){
    goto exit;
  }

  if(context->b_PAM)
  {
    const int is_token = strlen(psz_token);
    if(is_token > AUTHY_TOKEN_SIZE){
      psz_token = psz_token + (is_token - AUTHY_TOKEN_SIZE);
    } else {
      goto exit;
    }
  }
  debug(context->verb, __LINE__, "username=%s, token=%s ", psz_username, psz_token); 
  /* make a better use of envp to set the configuration file */
  if(get_authy_ID(AUTHY_VPN_CONF, psz_username, psz_common_name, psz_authy_ID) == FAILURE){
    goto exit;
  }
  debug(context->verb, __LINE__, "and AUTHY_ID=%s\n", psz_authy_ID);

  if(!(verify((const char *) context->psz_API_url, (const char *) context->psz_API_key, psz_token, psz_authy_ID, psz_response) == SUCCESS &&
       parse_response(psz_response) == SUCCESS)){
    goto exit;
  }

  i_status = SUCCESS;

exit:
  

  p_file_auth = fopen(psz_control, "w");
  /* set the control file to '1' if suceed or to '0' if fail */
  if(i_status != SUCCESS){
  debug(context->verb, __LINE__, "[Authy] Auth finished. Result: auth failed for username %s with token %s\n", psz_username, psz_token);
    fprintf(p_file_auth, "0");
  } else {
    debug(context->verb, __LINE__, "[Authy] Auth finished. Result: auth success for username %s\n", psz_username);
    fprintf(p_file_auth, "1");
  }

  memset(psz_token, 0, (strlen(psz_token))); /* Avoid to letf some sensible bits */
  fclose(p_file_auth);
  free(psz_response);
  return i_status;
}

/*
 * Dispatcher
 * this is the function that is called when one of the registered functions of the vpn
 * is called
 */
OPENVPN_EXPORT int
openvpn_plugin_func_v1(openvpn_plugin_handle_t handle, const int type, const char *argv[], const char *envp[])
{
  struct plugin_context *context = (struct plugin_context *) handle;

  if(type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY){
    return authenticate(context, argv, envp);
  }

  return OPENVPN_PLUGIN_FUNC_ERROR; /* Not sure, but for now it should be an error if we handle other callbacks */
}

/*
 * Free the memory related with the context
 * This is call before openvpn stops the plugin
 */
OPENVPN_EXPORT void
openvpn_plugin_close_v1(openvpn_plugin_handle_t handle)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  free(context->psz_API_url);
  free(context->psz_API_key);
  free(context);
}
