#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openvpn-plugin.h"
#include "jsmn.h"
#include "authy_api.h"

#define SUCCESS 0
#define FAILURE 1

#define AUTHYTOKENSIZE 7

#define TOKEN_STRING(js, t, s)                          \
  (strncmp(js+(t).start, s, (t).end - (t).start) == 0   \
   && strlen(s) == (t).end - (t).start)

/*
  This state expects the following config line
  plugin authy-openvpn.so APIURL APIKEY
  where APIURL should be something like
  APIURL https://api.authy.com/protected/json
  APIKEY like d57d919d11e6b221c9bf6f7c882028f9
  PAM pam | nopam # it is nopam by default
  pam = 1;
  nopam = 0;
*/
struct plugin_context {
  char *pszAPIUrl;
  char *pszAPIKey;
  int bPAM;
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
 */
OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1(unsigned int *type_mask, const char *argv[],
                       const char *envp[])
{
  /* Context Allocation */
  struct plugin_context *context;

  context = (struct plugin_context *) calloc(1, sizeof(struct
                                                       plugin_context));

  if(argv[1] && argv[2])
    {
      context->pszAPIUrl = strdup(argv[1]);
      context->pszAPIKey = strdup(argv[2]);
      context->bPAM      = 0;
    }

  if (argv[3] && strcmp(argv[3], "pam") == SUCCESS)
    context->bPAM = 1;

  /* Set type_mask, a.k.a callbacks that we want to intercept */
  *type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);

  /* Cast and return the context */
  return (openvpn_plugin_handle_t) context;
}

static int
parse_response(char *pszResponse)
{
  int cnt;
  jsmn_parser parser;
  jsmn_init(&parser);
  jsmntok_t tokens[20];
  jsmn_parse(&parser, pszResponse, tokens, 20);

  /* success isn't always on the same place, look until 19 because it
     shouldn't be the last one because it won't be a key */
  for (cnt = 0; cnt < 19; ++cnt)
    {
      if(TOKEN_STRING(pszResponse, tokens[cnt], "success"))
        {
          if(TOKEN_STRING(pszResponse, tokens[cnt+1], "true"))
            return SUCCESS;
          else
            return FAILURE;
        }
    }
  return FAILURE;
}

static int
authenticate(struct plugin_context *context, const char *argv[], const char *envp[])
{
  int iStatus;
  char *pszToken, *pszControl, *pszAuthyID, *pszResponse;
  FILE *pFileAuth;

  iStatus = FAILURE;
  pszResponse   = (char *) calloc(255, sizeof(char));

  /* the common name is the AuthyID, this need to be setted on the
     client certificate */
  pszAuthyID = get_env("common_name", envp);
  /* the username is the TOKEN to let the user see the typed token */
  pszToken   = get_env("username", envp);
  pszControl = get_env("auth_control_file", envp);

  pFileAuth = fopen(pszControl, "w");

  if(!pszAuthyID || !pszToken || !pszControl)
    goto exit;

  if(context->bPAM)
    {
      const int iszToken = strlen(pszToken);
      if(iszToken > AUTHYTOKENSIZE)
        pszToken = pszToken + (iszToken - AUTHYTOKENSIZE);
      else
        goto exit;
    }

  if(!(verify((const char *) context->pszAPIUrl,
              (const char *) context->pszAPIKey,
              pszToken, pszAuthyID, pszResponse) == SUCCESS &&
       parse_response(pszResponse) == SUCCESS))
    goto exit;

  iStatus = SUCCESS;

 exit:
  /* set the control file to '1' if suceed or to '0' if fail */
  if(iStatus != SUCCESS)
    fprintf(pFileAuth, "0");
  else
    fprintf(pFileAuth, "1");

  fclose(pFileAuth);
  free(pszResponse);
  return iStatus;
}

/*
 * Dispatcher
 */
OPENVPN_EXPORT int
openvpn_plugin_func_v1(openvpn_plugin_handle_t handle, const int type,
                       const char *argv[], const char *envp[])
{
  struct plugin_context *context = (struct plugin_context *) handle;

  if(type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY)
    return authenticate(context, argv, envp);
  return OPENVPN_PLUGIN_FUNC_ERROR; /* Not sure, but for now it should
                                       be an error if we handle other callbacks */
}

/*
 * Free the memory related with the context
 */
OPENVPN_EXPORT void
openvpn_plugin_close_v1(openvpn_plugin_handle_t handle)
{
  struct plugin_context *context = (struct plugin_context *) handle;
  free(context->pszAPIUrl);
  free(context->pszAPIKey);
  free(context);
}
