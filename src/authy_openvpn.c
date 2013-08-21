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
#include <assert.h>
#include <curl/curl.h>


#include "vendor/jsmn/jsmn.h"

#include "openvpn-plugin.h"
#include "authy_conf.h"
#include "authy_api.h"
#include "utils.h"
#include "logger.h"
#include "constants.h"

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
  char *pszApiUrl;
  char *pszApiKey;
  int bPAM;
  int verbosity;
};



// Description
//
// Given an environmental variable name, search
// the envp array for its value, returning it
//
// Parameters
// 
//   name            - Name of the enviromental
//   envp            - The environment 
//
// Returns
// 
// The value of the env variable or NULL otherwise
// if not found
//
/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 * From openvpn/sample/sample-plugins/defer/simple.c
 */
static char *
getEnv(const char *name, const char *envp[])
{
  if (envp)
  {
    int i;
    const int nameLength = strlen(name);
    for (i = 0; envp[i]; ++i)
    {
      if (!strncmp (envp[i], name, nameLength))
      {
        const char *cp = envp[i] + nameLength;
        if (*cp == '=')
          return (char *) cp + 1;
      }
    }
  }
  return NULL;
}



// Description
//
// Registers the functions that we want to intercept (OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY)
// and initializes the plugin context that holds the api url, api key and if we are using PAM
//
// Parameters
// 
//   type_mask            - Name of the enviromental
//   argv                 - arguments
//   envp                 - The environment 
//
// Returns
// 
// The handle to the plugin
//
OPENVPN_EXPORT openvpn_plugin_handle_t
openvpn_plugin_open_v1(unsigned int *type_mask, 
                       const char *argv[], 
                       const char *envp[])
{
  /* Context Allocation */
  struct plugin_context *context;

  context = (struct plugin_context *) calloc(1, sizeof(struct plugin_context));
  
  if(NULL == context){
		trace(ERROR, __LINE__, "[Authy] Failed to allocate context\n");
    return (openvpn_plugin_handle_t)FAIL;
  }

  /* Save the verbosite level from env */
  const char *verbosity = getEnv("verb", envp);
  if (verbosity){
    context->verbosity = (int)strtol(verbosity, (char **)NULL, 10);
  }

  if(argv[1] && argv[2])
  {
    trace(DEBUG, __LINE__, "Plugin path = %s\n", argv[0]);
    trace(DEBUG, __LINE__, "Api URL = %s, api key %s\n", argv[1], argv[2]); 
    context->pszApiUrl = (char *) calloc(strlen(argv[1]) + 1, sizeof(char));
    strncpy(context->pszApiUrl, argv[1], strlen(argv[1]));

    context->pszApiKey = (char *) calloc(strlen(argv[2]) + 1, sizeof(char));
    strncpy(context->pszApiKey, argv[2], strlen(argv[2]));

    context->bPAM  = 0;
	
  }

  if (argv[3] && strncmp(argv[3], "pam", 3) == 0){
    context->bPAM = 1;
  }

  /* Set type_mask, a.k.a callbacks that we want to intercept */
  *type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);

  /* Cast and return the context */
  return (openvpn_plugin_handle_t) context;

}



// Description
//
//  Goes through the response body looking for success. Else assumes failure
//  
// Parameters
// 
//   pszRespone           - Response body in json format
//
// Returns
// 
// Standard RESULT
//
static RESULT
responseWasSuccessful(char *pszResponse)
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
    if(strncmp(pszResponse + (tokens[cnt]).start, "success", (tokens[cnt]).end - (tokens[cnt]).start) == 0)
    {
      if(strncmp(pszResponse + (tokens[cnt+1]).start, "true", (tokens[cnt+1]).end - (tokens[cnt+1]).start) == 0){
        return OK;
      } else {
        return FAIL;
      }
    }
  }
	return FAIL;
}



// Description
//
// This is real core of the plugin
// it handles the authentication agains Authy services
// using the password field to obtain the OTP
// and as other authentication plugins it sets its authenication status
// to the control file
//  
// Parameters
// 
//   pszRespone           - Response body in json format
//
// Returns
// 
// Standard RESULT
//
static RESULT
authenticate(struct plugin_context *context, 
             const char *argv[], 
             const char *envp[])
{
  RESULT r = FAIL;
  char *pszToken = NULL;
  char *pszControl = NULL;
  char *pszResponse = NULL;
  char *pszCommonName = NULL;
  char *pszUsername = NULL;
  char *pszAuthyId = NULL;
  FILE *fpAuthFile = NULL;
  char *pszTokenStartPosition = NULL;

  pszCommonName =  getEnv("common_name", envp);
  pszUsername    = getEnv("username", envp);
  pszToken       = getEnv("password", envp);
  pszControl     = getEnv("auth_control_file", envp);
  pszResponse    = calloc(CURL_MAX_WRITE_SIZE + 1, sizeof(char)); 
  pszAuthyId		 = calloc(MAX_AUTHY_ID_LENGTH + 1, sizeof(char));

  trace(INFO, __LINE__, "[Authy] Authy Two-Factor Authentication started\n");
	
  if(!pszCommonName || !pszToken || !pszUsername || !pszResponse || !pszControl || !pszAuthyId){
    r = FAIL;
    goto EXIT;
  }

  fpAuthFile = fopen(pszControl, "w");
  if(NULL == fpAuthFile){
    r = FAIL;
    goto EXIT;
  }  

  r = getAuthyIdAndValidateCommonName(pszAuthyId,
                                      AUTHY_VPN_CONF, 
                                      pszUsername, 
                                      pszCommonName); 
  if(FAILED(r)){
		trace(ERROR, __LINE__, "[Authy] Authentication failed because Authy ID was not found for %s\n", pszUsername);
    r = FAIL;
    goto EXIT;
  }
	

  // Here check if the user is trying to just request a phone call or an sms token.
  if (0 == strcmp(pszToken, "sms")){
    
    sms(context->pszApiUrl, pszAuthyId, context->pszApiKey, pszResponse);
    r = FAIL; //doing phone call always fails authentication
    goto EXIT;
  }
  else if(0 == strcmp(pszToken, "call")){

    call(context->pszApiUrl, pszAuthyId, context->pszApiKey, pszResponse);

     r = FAIL; //doing phone call always fails authentication
     goto EXIT;
  }
  
  //PAM Authentication: password is concatenated and separated by TOKEN_PASSWORD_SEPARATOR 
  if(TRUE == context->bPAM) 
  {
    pszTokenStartPosition = strrchr(pszToken, TOKEN_PASSWORD_SEPARATOR); 
    if (NULL == pszTokenStartPosition){
      trace(ERROR, __LINE__, "[Authy] PAM being used but password was not properly concatenated. Use [PASS]-[TOKEN]\n");
      r = FAIL;
      goto EXIT;
    }
		*pszTokenStartPosition = '\0'; // This 0 terminates the password so that pam.so gets only the password and not the token.
    pszToken = pszTokenStartPosition + 1;
  }

  trace(INFO, __LINE__, "[Authy] Authenticating username=%s, token=%s with AUTHY_ID=%s\n", pszUsername, pszToken, pszAuthyId); 
  r = verifyToken(context->pszApiUrl, 
                  pszToken, 
                  pszAuthyId, 
                  context->pszApiKey, 
                  pszResponse);

  if (SUCCESS(r) && SUCCESS(responseWasSuccessful(pszResponse))){
    r = OK;
    goto EXIT;
  }

  r = FAIL;

EXIT:
  
  if(fpAuthFile){
    /* set the control file to '1' if suceed or to '0' if fail */
    if(SUCCESS(r)){
      trace(INFO, __LINE__, "[Authy] Auth finished. Result: Authy success for username %s\n", pszUsername);
      fprintf(fpAuthFile, "1");
    } else {
      trace(INFO, __LINE__, "[Authy] Auth finished. Result: Authy failed for username %s with token %s\n", pszUsername, pszToken);
      fprintf(fpAuthFile, "0");
    }
    fclose(fpAuthFile); 
  }
	if(pszAuthyId) {cleanAndFree(pszAuthyId);}
  if(pszToken) { memset(pszToken, 0, (strlen(pszToken))); } // Cleanup any sensible data
  if(pszResponse) { cleanAndFree(pszResponse);};
  return r;
}



// Description
//
// This is the function that is called when one of the registered functions of the vpn
// are called
//  
// Parameters
// 
//   pszRespone           - Response body in json format
//
// Returns
// 
// Standard 0 is success, or something else otherwise.
//
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
  cleanAndFree(context->pszApiUrl);
  cleanAndFree(context->pszApiKey);
  free(context);
}
