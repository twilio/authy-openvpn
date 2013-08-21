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
// Allocates teh memory and build the URL of the enpoind including 
// params.
//
// Parameters
// 
// pResultUrl - A pointer to where the URL will be stored
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
buildUrl(char       *pszResultUrl, 
         const char *pszApiUrl, 
         const char *pszEndPoint,  
         const char *pszParams, 
         const char *pszApiKey)
{
  RESULT r = FAIL;

  size_t urlSize = calcUrlSize(pszApiUrl, 
                               pszEndPoint, 
                               pszParams, 
                               pszApiKey);

  pszResultUrl = calloc(urlSize, sizeof(char));
  if(NULL == pszResultUrl){
    trace(ERROR, __LINE__, "[Authy] Out of Memory: Malloc failed.");
    r = OUT_OF_MEMORY;
    goto EXIT;
  }

  snprintf(pszResultUrl, 
           urlSize, 
           "%s%s%s%s", 
           pszApiUrl, pszEndPoint, pszParams, pszApiKey);

  trace(DEBUG, __LINE__, "[Authy] BuilUrl pszResultUrl=%s\n.", pszResultUrl);
  r = OK;

EXIT:
  return r;
}


//
// Description
//
// This an auxiliar function that curl uses
// it redirects the output of the libcurl to a buffer
//
static size_t
customWriter(char *ptr, 
             size_t size, 
             size_t nmemb, 
             void *userdata)
{
  char *temp = (char *) userdata;
  memcpy(temp, ptr, (size_t) size * nmemb);
  return nmemb*size;
}


//
// Description
//
// Handles the request to the api
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
request(char *pszResultUrl, char *pszPostFields, char *pszResponse)
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
  curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, customWriter);
  curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pszResponse);
  
  curlResult = (int) curl_easy_perform(pCurl);
  if(curlResult != 0) {
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
  char *pszParams = "?api_k)y=";
  r = buildUrl(pszResultUrl, 
               pszApiUrl, 
               pszEndPoint,  
               pszParams, 
               pszApiKey);

  if(FAILED(r)){
    goto EXIT;
  }

  r = request(pszResultUrl, pszPostFields, pszResponse);
  
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

  r = buildUrl(pszResultUrl, 
               pszApiUrl, 
               pszEndPoint,  
               pszParams, 
               pszApiKey);

  if(FAILED(r)) {
    trace(INFO, __LINE__, "[Authy] URL for Token verification failed\n");
    goto EXIT;
  }

  r = request(pszResultUrl, NULL, pszResponse); //GET request, postFields are NULL
  
  if(FAILED(r)) {
    trace(INFO, __LINE__, "[Authy] Token request verification failed\n");
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
// pszApiKey      - The Authy API key
// pszResponse    - Pointer to where the response will be stored.
//
// Returns
//
// Standard RESULT  
//
extern RESULT
requestSms(const char *pszApiUrl, 
            char *pszAuthyId, 
            const char *pszApiKey, 
            char *pszResponse)
{
  int r = FAIL;
  size_t endPointSize = 0;
  char *pszResultUrl = NULL;
  char *pszEndPoint = NULL;
  char *pszParams = "?api_key=";

  endPointSize = strlen("/sms/") + strlen(pszAuthyId) + 1;
  pszEndPoint = calloc(endPointSize, sizeof(char));
  if(NULL == pszEndPoint){
    r = OUT_OF_MEMORY;
    goto EXIT;
  }

  snprintf(pszEndPoint, endPointSize, "/sms/%s", pszAuthyId);

  r = buildUrl(pszResultUrl, 
               pszApiUrl, 
               pszEndPoint,  
               pszParams, 
               pszApiKey);

  if(FAILED(r)) {
    r = FAIL;
    goto EXIT;
  }

  r = request(pszResultUrl, NULL, pszResponse);

EXIT:
  cleanAndFree(pszResultUrl);
  pszResultUrl = NULL;
  cleanAndFree(pszEndPoint);
  pszEndPoint = NULL;

  return r;
}
