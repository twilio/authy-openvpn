//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   authy_api.c
//
// Abstract
//
// Implements the public Authy API using json. Uses CURL to do multi-platform HTTPS request.
//
// registerUser
// verifyToken
// requestSMS
//
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------


#include <stdarg.h>
#include <curl/curl.h>
#include <assert.h>
#include "utils.h"
#include "logger.h"
#include "authy_api.h"

#ifdef WIN32
#define snprintf _snprintf
#endif


//
// Description
//
// Given the url, endpoint, params and keys, calculates the size
// of the URL
//
// Parameters
//
// pszApiUrl   - Server URL
// pszEndPoint - The API endpoint including format
// pszParams   - The endpoint params.
// pszApiKey   - The Authy API key.
//
// Returns
//
// The size of the URL to be allocated.
//
//

static size_t
calcUrlSize(const char *pszApiUrl,
            const char *pszEndPoint,
            const char *pszParams,
            const char *pszApiKey)
{
  return strlen(pszApiUrl) + strlen(pszEndPoint) + strlen(pszParams) + strlen(pszApiKey) + 1;
}

//
// Description
//
// Allocates the memory and build the URL of the enpoind including
// params.
//
// Parameters
//
// pResultUrl -  A pointer to the pointer were the URL will be stored.
// pszApiUrl   - Server URL
// pszEndPoint - The API endpoint including format
// pszParams   - The endpoint params.
// pszApiKey   - The Authy API key.
//
// Returns
//
// Standard RESULT
//
RESULT
buildUrl(__out char **ppszResultUrl,
         const char *pszApiUrl,
         const char *pszEndPoint,
         const char *pszParams,
         const char *pszApiKey)
{
  assert(ppszResultUrl != NULL);

  RESULT r = FAIL;

  size_t urlSize = calcUrlSize(pszApiUrl,
                               pszEndPoint,
                               pszParams,
                               pszApiKey);


  *ppszResultUrl = calloc(urlSize, sizeof(char));
  if(NULL == *ppszResultUrl){
    trace(ERROR, __LINE__, "[Authy] Out of Memory: Malloc failed.");
    r = OUT_OF_MEMORY;
    goto EXIT;
  }

  snprintf(*ppszResultUrl,
           urlSize,
           "%s%s%s%s",
           pszApiUrl, pszEndPoint, pszParams, pszApiKey);

  trace(DEBUG, __LINE__, "[Authy] buildUrl pszResultUrl=%s\n.", *ppszResultUrl);
  r = OK;

EXIT:
  return r;
}



//
// Description
//
// curl custom writer. Implements the prototype:
// prototype: size_t function( char *ptr, size_t size, size_t nmemb, void *userdata);
//
// Parameters
//
// ptr         - Rough data with size size * nmemb. Not zero terminated
// size        - size of each member of ptr
// nmemb       - number of members
// userdata    - pointer to were the date is written too. Max write is CURL_MAX_WRITE_SIZE
//               We allocate userdate 0 termited from the start.
//
// Returns
//
// Ammount of data that was written to userdata. Else curl will raise an
// error.
//
//
static size_t
curlWriter(char *ptr,
             size_t size,
             size_t nmemb,
             void *userdata)
{
  memcpy(userdata, ptr, (size_t) size * nmemb);
  return nmemb*size;
}


//
// Description
//
// Handles the http request to the api
// it knows when to do a GET or a POST based
// on the present of pszPostFields
//
// Parameters
//
// pszResultUrl         - The full URL
// pszPostFields  - POST fields if it's a POST request or NULL for GET request
// pszEndPoint - The API endpoint including format
// pszParams   - The endpoint params.
// pszApiKey   - The Authy API key.
//
// Returns
//
// Standard RESULT
//
RESULT
doHttpRequest(char *pszResultUrl, char *pszPostFields, char *pszResponse)
{
  RESULT r = FAIL;
  CURL *pCurl = NULL;
  int curlResult = -1;

  curl_global_init(CURL_GLOBAL_ALL);

  pCurl = curl_easy_init();
  if(!pCurl){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] CURL failed to initialize");
    goto EXIT;
  }

  curl_easy_setopt(pCurl, CURLOPT_URL, pszResultUrl);

  if(pszPostFields){// POST REQUEST
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pszPostFields);
  }

#ifdef WIN32
  curl_easy_setopt(p_curl, CURLOPT_CAINFO, "curl-bundle-ca.crt");
#endif

  curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 1L); //verify PEER certificate
  curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 2L); //verify HOST certificate
  curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, curlWriter);
  curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pszResponse);

  curlResult = (int) curl_easy_perform(pCurl);
  if(0 != curlResult) {
    trace(ERROR, __LINE__, "Curl failed with code %d", curlResult);
    r = FAIL;
    goto EXIT;
  }

  trace(INFO, __LINE__, "[Authy] Curl response: Body=%s\n", pszResponse);
  r = OK;

EXIT:

#ifdef WIN32
  trace(DEBUG, __LINE__, "[Authy] Can't clean curl, curl easy cleanup doesn't work on Windows");
#else
  if(pCurl){
    curl_easy_cleanup(pCurl);
  }
#endif

  return r;
}


//
// Description
//
// Calls the new user Authy API API
//
// Parameters
//
// pszApiUrl    - The server URL
// pszPostFields  - POST fields if it's a POST request
// pszEndPoint - The API endpoint including format
// pszParams   - The endpoint params.
// pszApiKey   - The Authy API key.
//
// Returns
//
// Standard RESULT
//
extern RESULT
registerUser(const char *pszApiUrl,
              char *pszPostFields,
              const char *pszApiKey,
              char *pszResponse)
{
  int r = FAIL;
  char *pszResultUrl = NULL;
  char *pszEndPoint = "/users/new";
  char *pszParams = "?api_key=";

  r = buildUrl(&pszResultUrl,
               pszApiUrl,
               pszEndPoint,
               pszParams,
               pszApiKey);

  if(FAILED(r)){
    goto EXIT;
  }

  r = doHttpRequest(pszResultUrl, pszPostFields, pszResponse);

  // Clean memory used in the request
  cleanAndFree(pszResultUrl);
  pszResultUrl = NULL;

  if(FAILED(r)){
    trace(ERROR, __LINE__, "[Authy] User Registration Failed\n");
    goto EXIT;
  }

  r = OK;

EXIT:
  return r;
}



//
// Description
//
// Calls the verify Authy API using force=true
//
// Does a GET to https://api.authy.com/protected/{FORMAT}/verify/{TOKEN}/{AUTHY_ID}?force=true&api_key={KEY}
// Parameters
//
// pszApiUrl      - The server URL
// pszToken       - The token entered by the user
// pszAuthyId     - The Authy ID fo the user
// pszApiKey      - The Authy API key
// pszResponse    - Pointer to where the response will be stored.
//
// Returns
//
// Standard RESULT
//
extern RESULT
verifyToken(const char *pszApiUrl,
            char *pszToken,
            char *pszAuthyId,
            const char *pszApiKey,
            char *pszResponse)
{
  RESULT r = FAIL;
  size_t endPointSize = 0;
  char *pszResultUrl = NULL;
  char *pszEndPoint = NULL;
  char *pszParams = "?force=true&api_key=";

  endPointSize = strlen("/verify/") + strlen(pszToken) + strlen("/") + strlen(pszAuthyId) + 1;
  pszEndPoint = calloc(endPointSize, sizeof(char));
  if(!pszEndPoint){
    r = FAIL;
    goto EXIT;
  }

  snprintf(pszEndPoint, endPointSize, "/verify/%s/%s", pszToken, pszAuthyId);

  r = buildUrl(&pszResultUrl,
               pszApiUrl,
               pszEndPoint,
               pszParams,
               pszApiKey);

  if(FAILED(r)) {
    trace(INFO, __LINE__, "[Authy] URL for Token verification failed\n");
    goto EXIT;
  }

  r = doHttpRequest(pszResultUrl, NULL, pszResponse); //GET request, postFields are NULL

  if(FAILED(r)) {
    trace(INFO, __LINE__, "[Authy] Token request verification failed.\n");
    goto EXIT;
  }


EXIT:
  cleanAndFree(pszResultUrl);
  pszResultUrl = NULL;
  cleanAndFree(pszEndPoint);
  pszEndPoint = NULL;

  return r;
}


///
// Description
//
// Calls the request SMS Authy API.
//
//
// Parameters
//
// pszApiUrl      - The server URL
// pszAuthyId     - The Authy ID fo the user
// pszVia         - This is the way, either 'call' or 'sms'
// pszApiKey      - The Authy API key
// pszResponse    - Pointer to where the response will be stored.
//
// Returns
//
// Standard RESULT
//
extern RESULT
sendTokenToUser(const char *pszApiUrl,
            char *pszAuthyId,
            char *pszVia,
            const char *pszApiKey,
            char *pszResponse)
{
  int r = FAIL;
  size_t endPointSize = 0;
  char *pszResultUrl = NULL;
  char *pszEndPoint = NULL;
  char *pszParams = "?api_key=";

  // /call/1 or /sms/1. Including the 2 slashes.
  endPointSize = 2 + strlen(pszVia) + strlen(pszAuthyId) + 1;
  pszEndPoint = calloc(endPointSize, sizeof(char));
  if(NULL == pszEndPoint){
    r = OUT_OF_MEMORY;
    goto EXIT;
  }

  snprintf(pszEndPoint, endPointSize, "/%s/%s", pszVia, pszAuthyId);
  r = buildUrl(&pszResultUrl,
               pszApiUrl,
               pszEndPoint,
               pszParams,
               pszApiKey);

  if(FAILED(r)) {
    r = FAIL;
    goto EXIT;
  }

  trace(INFO, __LINE__, "[Authy] Requesting %sfor Authy ID\n", pszVia, pszAuthyId);
  r = doHttpRequest(pszResultUrl, NULL, pszResponse);

EXIT:
  cleanAndFree(pszResultUrl);
  pszResultUrl = NULL;
  cleanAndFree(pszEndPoint);
  pszEndPoint = NULL;

  return r;
}

///
// Description
//
// Calls the request SMS Authy API.
//
//
// Parameters
//
// pszApiUrl      - The server URL
// pszAuthyId     - The Authy ID fo the user
// pszApiKey      - The Authy API key
// pszResponse    - Pointer to where the response will be stored.
//
// Returns
//
// Standard RESULT
//
extern RESULT
sms(const char *pszApiUrl,
          char *pszAuthyId,
    const char *pszApiKey,
          char *pszResponse)
{
  int r = FAIL;

  char *pszVia = "sms";
  r = sendTokenToUser(pszApiUrl,
                      pszAuthyId,
                      pszVia,
                      pszApiKey,
                      pszResponse);

  if (FAILED(r)){
    trace(ERROR, __LINE__, "[AUTHY] Error sendingsms token to user\n");
    r = FAIL;
    goto EXIT;
  }

  r = OK;

EXIT:
  return r;
}


///
// Description
//
// Calls the request call Authy API.
//
//
// Parameters
//
// pszApiUrl      - The server URL
// pszAuthyId     - The Authy ID fo the user
// pszApiKey      - The Authy API key
// pszResponse    - Pointer to where the response will be stored.
//
// Returns
//
// Standard RESULT
//
extern RESULT
call(const char *pszApiUrl,
            char *pszAuthyId,
            const char *pszApiKey,
            char *pszResponse)
{
  int r = FAIL;

  char *pszVia = "call";
  r = sendTokenToUser(pszApiUrl,
                      pszAuthyId,
                      pszVia,
                      pszApiKey,
                      pszResponse);

  if (FAILED(r)){
    trace(ERROR, __LINE__, "[AUTHY] Error trying to call the user\n");
    r = FAIL;
    goto EXIT;
  }

  r = OK;

EXIT:
  return r;
}



