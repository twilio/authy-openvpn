#include "authy_api.h"

#define SUCCESS 0
#define FAILURE 1

/* clean and free */
static void
clean_and_free(void * p_target)
{
  memset(p_target, 0, sizeof p_target);
  free(p_target);
  p_target = NULL;
}

/* returns the needed size to built the url */
static int
url_size(const char *psz_API_url, const char *psz_end_point,
         const char *psz_API_key)
{
  return strlen(psz_API_url) + strlen(psz_end_point) +
    strlen("?api_key=") + strlen(psz_API_key) + 1;
}

/* It returns the actual size of the url */
static int
url_builder(const char *psz_API_url, const char *psz_API_key,
            char *psz_end_point, char *psz_url)
{
  return sprintf(psz_url, "%s%s?api_key=%s", psz_API_url, psz_end_point, psz_API_key);
}

static size_t
custom_writer(char *ptr, size_t size, size_t nmemb,
              void *userdata)
{
  char *temp = (char *) userdata;
  memcpy(temp, ptr, (size_t) size * nmemb);
  return nmemb*size;
}

static int
request(char *psz_url, char *psz_post_fields, char *psz_response)
{
  CURL *p_curl;
  int i_res;

  curl_global_init(CURL_GLOBAL_ALL);

  p_curl = curl_easy_init();

  i_res = FAILURE;

  if(!p_curl)
    goto exit;

  curl_easy_setopt(p_curl, CURLOPT_URL, psz_url);

  if(psz_post_fields)
    curl_easy_setopt(p_curl, CURLOPT_POSTFIELDS, psz_post_fields);

  /* This option determines whether curl verifies the authenticity
     of the peer's certificate */
  curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYPEER, 1L);
  /* curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); */

  curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, custom_writer);
  curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, psz_response);

  i_res = (int) curl_easy_perform(p_curl);

 exit:
  /* Always cleanup otherwise it will lead to memoryleaks */
  curl_easy_cleanup(p_curl);

  return i_res;
}

extern int
register_user(const char *psz_API_url, const char *psz_API_key,
              char *psz_post_fields, char *psz_response)
{
  int i_res = FAILURE;
  char *psz_url = NULL, *psz_end_point = "/users/new";

  psz_url = (char *) malloc(url_size(psz_API_url, psz_end_point,
                                     psz_API_key));
  if(!psz_url)
    goto exit;

  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url);

  i_res = request(psz_url, psz_post_fields, psz_response);

 exit:
  clean_and_free(psz_url);

  return i_res;
}

extern int
verify(const char *psz_API_url, const char *psz_API_key,
       char *psz_token, char *psz_authy_ID, char *psz_response)
{
  int i_res = FAILURE;
  char *psz_url = NULL, *psz_end_point = NULL;

  psz_end_point = (char *) malloc(strlen("/verify/") + strlen(psz_token)
                                  + strlen("/") +
                                  strlen(psz_authy_ID));
  if(!psz_end_point)
    goto exit;

  sprintf(psz_end_point, "/verify/%s/%s", psz_token, psz_authy_ID);

  psz_url = (char *) malloc(url_size(psz_API_url, psz_end_point,
                                     psz_API_key));

  if(!psz_url)
    goto exit;
  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url);

  i_res = request(psz_url, NULL, psz_response);

 exit:
  clean_and_free(psz_url);
  clean_and_free(psz_end_point);

  return i_res;
}

extern int
request_sms(const char *psz_API_url, const char *psz_API_key,
            char *psz_authy_ID, char *psz_response)
{
  int i_res = FAILURE;
  char *psz_url = NULL, *psz_end_point = NULL;

  psz_end_point = (char *) malloc(strlen("/sms/") +
                                  strlen(psz_authy_ID));
  if(!psz_end_point)
    goto exit;

  sprintf(psz_end_point, "/sms/%s", psz_authy_ID);

  psz_url = (char *) malloc(url_size(psz_API_url, psz_end_point,
                                     psz_API_key));

  if(!psz_url)
    goto exit;

  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url);

  i_res = request(psz_url, NULL, psz_response);

 exit:
  clean_and_free(psz_url);
  clean_and_free(psz_end_point);

  return i_res;
}
