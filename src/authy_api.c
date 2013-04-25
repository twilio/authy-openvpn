#include "authy_api.h"

#define NSizeAux strlen("?api_key=")
#define ALLOCURL (char *) malloc(strlen(pszAPIUrl) + strlen(pszEndPoint) +NSizeAux + strlen(pszAPIKey) + 1)

static size_t
custom_writer(char *ptr, size_t size, size_t nmemb,
              void *userdata)
{
  char *temp = (char *) userdata;
  memcpy(temp, ptr, (size_t) size * nmemb);
  return nmemb*size;
}

static int
curl_request(char *pszUrl, char *pszPostFields, char *pszResponse)
{
  CURL *pCurl;
  int iRes;

  curl_global_init(CURL_GLOBAL_ALL);

  pCurl = curl_easy_init();

  iRes = -1;

  if(pCurl)
    {
      curl_easy_setopt(pCurl, CURLOPT_URL, pszUrl);

      if(pszPostFields)
        curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pszPostFields);

      /* This option determines whether curl verifies the authenticity
         of the peer's certificate */
      curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 1L);
      /* curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); */

      curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, custom_writer);
      curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pszResponse);

      iRes = (int) curl_easy_perform(pCurl);
      /* Always cleanup otherwise it will lead to memoryleaks */
      curl_easy_cleanup(pCurl);
    }
  return iRes;
}

/* It returns the actual size of the url */

static int
url_builder(const char *pszAPIUrl, const char *pszAPIKey,
            char *pszEndPoint, char *pszUrl)
{
  return sprintf(pszUrl, "%s%s?api_key=%s", pszAPIUrl, pszEndPoint, pszAPIKey);
}

extern int
register_user(const char *pszAPIUrl, const char *pszAPIKey,
              char *pszPostFields, char *pszResponse)
{
  int iRes;
  char *pszUrl, *pszEndPoint = "/users/new";

  pszUrl = ALLOCURL;
  url_builder(pszAPIUrl, pszAPIKey, pszEndPoint, pszUrl);

  iRes = curl_request(pszUrl, pszPostFields, pszResponse);
  free(pszUrl);
  return iRes;
}

extern int
verify(const char *pszAPIUrl, const char *pszAPIKey,
       char *pszToken, char *pszAuthyID, char *pszResponse)
{
  int iRes;
  char *pszUrl, *pszEndPoint;
  pszEndPoint = (char *) malloc(strlen("/verify/") + strlen(pszToken)
                                + strlen("/") + strlen(pszAuthyID));
  sprintf(pszEndPoint, "/verify/%s/%s", pszToken, pszAuthyID);

  pszUrl = ALLOCURL;
  url_builder(pszAPIUrl, pszAPIKey, pszEndPoint, pszUrl);

  iRes = curl_request(pszUrl, NULL, pszResponse);
  free(pszUrl);
  free(pszEndPoint);
  return iRes;
}

extern int
request_sms(const char *pszAPIUrl, const char *pszAPIKey,
            char *pszAuthyID, char *pszResponse)
{
  int iRes;
  char *pszUrl, *pszEndPoint;
  pszEndPoint = (char *) malloc(strlen("/sms/") + strlen(pszAuthyID));
  sprintf(pszEndPoint, "/sms/%s", pszAuthyID);

  pszUrl = ALLOCURL;
  url_builder(pszAPIUrl, pszAPIKey, pszEndPoint, pszUrl);

  iRes = curl_request(pszUrl, NULL, pszResponse);
  free(pszUrl);
  free(pszEndPoint);
  return iRes;
}
