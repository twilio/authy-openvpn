//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   utils.c
//
// Abstract
//
// Utility Functions
//
//
// History
//
//  8/1/2013    dpalacio    Created
//  9/2/2015    serargz     Edited
//
//------------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef WIN32
#include "sys/utsname.h"
#endif

#include "openvpn-plugin.h"
#include "constants.h"
#include "custom_types.h"
#include "logger.h"

//
// Description
//
// Memsets the pointer string to 0's and free's it if is not NULL.
// Ignores it otherwise.
//
// Parameters
//
//   pTarget - The pointer being free'd
//
// Returns
// void
//
void
cleanAndFree(void *pszPtr)
{
	size_t length = 0;
  if(NULL != pszPtr)  {
		length = strlen(pszPtr);
    memset(pszPtr, 0, sizeof(char)*length);
    free(pszPtr);
  }
}



// Description
//
// Remove spaces from a NULL terminated String
//
// Parameters
//
//   pszString - A NULL terminated string containing spaces
//
// Returns
//
// a pointer to the beginning of the same string.
//
char *
removeSpaces(char *pszString)
{
  assert(pszString != NULL);

  char* i = pszString;
  char* j = pszString;

  while(*j != '\0')
  {
    if(*i != ' ' && *i != '\n' && *i != '\r' && *i != '\t'){
      *i = *j;
      i++;
    }
    j++;
  }
  *i = '\0';

  return pszString;
}

// Description
//
// Takes a token String entered by the end user and sanitizes
// the input to make sure it's only numeric and is less than
// 12 characters.
//
// Parameters
//
//   pszString - A NULL terminated string
//
// Returns
//
//   The sanitized token String.
//
BOOL
isTokenSafe(char *pszToken)
{
  size_t len = strlen(pszToken);
  BOOL isNumeric = TRUE;

  int i;
  for(i = 0; i < len; i++)
  {
    if(0 == isdigit(pszToken[i]))
    {
      isNumeric = FALSE;
    }
  }

  if(isNumeric && len >= MIN_TOKEN_LENGTH && len <= MAX_TOKEN_LENGTH) {
    return TRUE;
  }

  if(FALSE == isNumeric)
  {
    trace(INFO, __LINE__, "[Authy] Possible hack attempt, token is not numeric\n");
  }

  trace(INFO, __LINE__, "[Authy] Token length is invalid: %i\n", len);
  return FALSE;
}

// Description
//
// Generates a user agent string for the plugin.
//
// Returns
//
//   A string with the user agent.
char *
getUserAgent()
{
  size_t userAgentSize = 0;
  size_t systemInfoLength = 0;
  ssize_t read = 0;
  char *pszUserAgent = NULL;
  char *pszSystemInfo = NULL;

#ifdef WIN32
  systemInfoLength = strlen("Windows");
  pszSystemInfo = calloc(systemInfoLength + 1, sizeof(char));
  if(NULL == pszSystemInfo)
  {
    trace(ERROR, __LINE__, "[Authy] Could not allocate space for System Info\n");
    goto EXIT;
  }
  snprintf(pszSystemInfo, systemInfoLength, "Windows");
#else
  struct utsname unameData;
  if(-1 == uname(&unameData))
  {
    trace(INFO, __LINE__, "[Authy] Could not fetch system info\n");
    pszSystemInfo = calloc(strlen("Unknown") + 1, sizeof(char));
    if(NULL == pszSystemInfo)
    {
      trace(ERROR, __LINE__, "[Authy] Could not allocate space for System Info\n");
      goto EXIT;
    }
    snprintf(pszSystemInfo, strlen("Unknown"), "Unknown");
  }
  else
  {
    systemInfoLength = strlen(unameData.sysname) + strlen(" ") + strlen(unameData.release);
    pszSystemInfo = calloc(systemInfoLength + 1, sizeof(char));
    if(NULL == pszSystemInfo)
    {
      trace(ERROR, __LINE__, "[Authy] Could not allocate space for System Info\n");
      goto EXIT;
    }
    snprintf(pszSystemInfo, systemInfoLength, "%s %s", unameData.sysname, unameData.release);
  }
#endif

  userAgentSize = strlen("AuthyOpenVPN/ ()") + strlen(AUTHY_OPENVPN_VERSION) + strlen(pszSystemInfo);
  pszUserAgent = calloc(userAgentSize + 1, sizeof(char));
  if(NULL == pszUserAgent)
  {
    trace(ERROR, __LINE__, "[Authy] Could not allocate space for User Agent\n");
    goto EXIT;
  }

  snprintf(pszUserAgent, userAgentSize, "AuthyOpenVPN/%s (%s)", AUTHY_OPENVPN_VERSION, pszSystemInfo );

EXIT:
  if(pszSystemInfo)
  {
    free(pszSystemInfo);
  }

  return pszUserAgent;
}
