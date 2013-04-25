#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openvpn-plugin.h"
#include "jsmn.h"
#include "authy_api.h"

/* bool definitions */
#define bool int
#define true 1
#define false 0


/*
  This state expects the following config line
  plugin authy-openvpn.so APIURL APIKEY
  where APIURL should be something like
  https://api.authy.com/protected/json
  and APIKEY like d57d919d11e6b221c9bf6f7c882028f9
*/
struct plugin_context {
  char *pszAPIUrl;
  char *pszAPIKey;
};

/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 * From openvpn/sample/sample-plugins/defer/simple.c
 */
static const char *
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
                return cp + 1;
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
    }

  /* Set type_mask, a.k.a callbacks that we want to intercept */
  *type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);

  /* Cast and return the context */
  return (openvpn_plugin_handle_t) context;
}

static int
authenticate(struct plugin_context *context, const char *argv[], const char *envp[])
{
  int iStatus;
  const char *pszUsername, *pszPassword, *pszControl;
  char *pszResponse;

  const char *pszCommonName;                 /* delete me */
  pszCommonName = get_env("common_name", envp); /* delete me */
  pszUsername = get_env("username", envp); /* this should be the authy id */
  pszPassword = get_env("password", envp); /* this should be the token */
  pszControl  = get_env("auth_control_file", envp);

  /* check env vars aren't null */
  if(!pszUsername || !pszPassword || !pszControl)
    return OPENVPN_PLUGIN_FUNC_ERROR;

  printf(" common name = %s\n user name = %s\n password = %s\n control = %s\n",
         pszCommonName, pszUsername, pszPassword, pszControl);
  /* TODO link authy api */
  pszResponse = (char *) malloc(1024);
  iStatus = verify((const char *) context->pszAPIUrl,
                   (const char *) context->pszAPIKey,
                   pszPassword, pszUsername, pszResponse);
  printf(" response = %s\n", pszResponse);
  /* FILE * pFileAuth = fopen(pszControl, "w"); */
  /* fprintf(pFileAuth, "1");  */

  /* TODO parse pszResponse and check iStatus */
  /* doit must set at the end the control file to '1' if suceed or to '0'
     if fail*/
  free(pszResponse);
  return OPENVPN_PLUGIN_FUNC_DEFERRED; /* for now just to debug the
                                          envp */
}

/*wd
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
