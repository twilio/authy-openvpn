//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   authy_api.h
//
// Abstract
// 
// Implements the public Authy API using json. Uses CURL to do multi-platform HTTPS request.
//
// History
//
//  8/1/2013    dpalacio    Modified
//
//------------------------------------------------------------------------------

#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "custom_types.h"

extern RESULT
registerUser(const char *pszApiUrl, 
              char *pszPostFields, 
              const char *pszApiKey, 
              char *pszResponse);


extern RESULT
verifyToken(const char *pszApiUrl, 
            char *pszToken, 
            char *pszAuthyId, 
            const char *pszApiKey, 
            char *pszResponse);


extern RESULT
sms(const char *pszApiUrl, 
          char *pszAuthyId, 
          const char *pszApiKey, 
          char *pszResponse);

extern RESULT
call(const char *pszApiUrl, 
          char *pszAuthyId, 
          const char *pszApiKey, 
          char *pszResponse);
