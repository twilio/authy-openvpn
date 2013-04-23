#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openvpn-plugin.h"

/* bool definitions */
#define bool int
#define true 1
#define false 0


/* Pending: define context, a.k.a state */

struct plugin_context {
};

/*
 * Given an environmental variable name, search
 * the envp array for its value, returning it
 * if found or NULL otherwise.
 * From openvpn/sample/sample-plugins/defer/simple.c
 */
static const char *
get_env (const char *name, const char *envp[])
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
openvpn_plugin_open_v1 (unsigned int *type_mask, const char *argv[],
                        const char *envp[])
{
  /* Context Allocation */

  /* Set type_mask, a.k.a callbacks that we want to intercept */
  *type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);
    
  /* Cast and return the context */
}

static int
authenticate(plugin_context *context, const char *argv[], const char *envp)
{
  /* doit */
}

/*
 * Dispatcher
 */
OPENVPN_EXPORT int
openvpn_plugin_func_v1 (openvpn_plugin_handle_t handle, const int type,
			const char *argv[], const char *envp[])
{

  /* Pending: set/cast handle to context */
  
  if(type == OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY)
    return authenticate(context, argv envp);
  return OPENVPN_PLUGIN_FUNC_ERROR; /* Not sure, but for now it should
                                       be an error if we handle other callbacks */
}

/*
 * Free the memory related with the context
 */
OPENVPN_EXPORT void
openvpn_plugin_close_v1 (openvpn_plugin_handle_t handle)
{
  /* struct plugin_context *context = (struct plugin_context *) handle; */
  /* free (context); */
}
