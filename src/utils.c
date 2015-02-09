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
char *
truncateAndSanitizeToken(char *pszToken)
{
  size_t len = strnlen(pszToken, MAX_TOKEN_LENGTH);
  if(len < MIN_TOKEN_LENGTH) {
    trace(INFO, __LINE__, "[Authy] Token entered by user is too short: %i", len);
    pszToken = "000000";
  }
  pszToken[len] = '\0'; // Truncate the token to max token length
  int i;
  for(i = 0; i < len; i++)
  {
    if(0 == isdigit(pszToken[i]))
    {
      trace(INFO, __LINE__, "[Authy] Possible hack attempt, token is not numeric");
      pszToken[i] = '0'; // Sanitize non digit character
    }
  }

  return pszToken;
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
  char *pszUserAgent = NULL;

#ifdef WIN32
  userAgentSize = strlen("AuthyOpenVPN/x (Windows)") + 10; // Additional space for version changes
  pszUserAgent = calloc(userAgentSize, sizeof(char));
  sprintf(pszUserAgent, "AuthyOpenVPN/%i (Windows)", OPENVPN_PLUGIN_VERSION);
#else
  struct utsname unameData;
  uname(&unameData);
  userAgentSize = strlen("AuthyOpenVPN/x ( )") + strlen(unameData.sysname) + strlen(unameData.release) + 10; // Additional space for version changes
  pszUserAgent = calloc(userAgentSize, sizeof(char));

  sprintf(pszUserAgent, "AuthyOpenVPN/%i (%s %s)", OPENVPN_PLUGIN_VERSION, unameData.sysname, unameData.release );
#endif

  return pszUserAgent;
}
