#include <stdarg.h>
#include <curl/curl.h>

#include "authy_api.h"
#ifdef WIN32
#define snprintf _snprintf
#endif

#define SUCCESS 0
#define FAILURE 1

#define VERB 4
static int debug(const int line, const char *format, ...)
{
	if(VERB >= 4)
	{
		printf("Authy Plugin: %d\t", line);
		va_list arg;
		int done;

        va_start(arg, format);
        done = vfprintf(stderr, format, arg);
        va_end (arg);
		fflush(stderr);
        return done;
	}
	return 0;
}

/*
 * clean and free
 * Sets the used memory to null and then frees it
 */
char *
clean_and_free(void * p_target)
{
  if(p_target != NULL) {
    memset(p_target, 0, sizeof p_target);
    free(p_target);
  }
  return NULL;
}

/*
 * url size
 * calls the size need for an api url
 */
static size_t
calc_url_size(const char *psz_API_url, const char *psz_end_point, const char *psz_API_key, const char *psz_params)
{
  return strlen(psz_API_url) + strlen(psz_end_point) + strlen(psz_params) + strlen(psz_API_key) + 1;
}

/* url builder
 * builds the url and save it to psz_url
 * and returns the actual size of the url
 */
static char*
url_builder(const char *psz_API_url, const char *psz_API_key, char *psz_end_point, char *psz_params)
{
  char *psz_url = NULL;
  size_t url_size = calc_url_size(psz_API_url, psz_end_point, psz_API_key, psz_params);

  psz_url = calloc(url_size, sizeof(char));
  if(!psz_url){
    goto error;
  }

  snprintf(psz_url, url_size, "%s%s%s%s", psz_API_url, psz_end_point, psz_params, psz_API_key);

  return psz_url;

error:
  debug(__LINE__, "[Authy] memory error: malloc failed.");
  return NULL;
}

/*
 * custom writer
 * this an auxiliar function for curl
 * it redirects the output of the libcurl to a buffer
 */
static size_t
custom_writer(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  char *temp = (char *) userdata;
  memcpy(temp, ptr, (size_t) size * nmemb);
  return nmemb*size;
}

/*
 * request
 * this method handles the request to the api
 * it knows when to make a GET or a POST based
 * on the present of psz_post_fields
 *
 * Returns:
 *   SUCCESS: request was success.
 *   FAILURE: curl failed.
 */
static int
request(char *psz_url, char *psz_post_fields, char *psz_response)
{
  CURL *p_curl;
  int res = FAILURE;

  curl_global_init(CURL_GLOBAL_ALL);

  p_curl = curl_easy_init();
  if(!p_curl){
    goto exit;
  }

  curl_easy_setopt(p_curl, CURLOPT_URL, psz_url);

  if(psz_post_fields){
    curl_easy_setopt(p_curl, CURLOPT_POSTFIELDS, psz_post_fields);
  }
  /* These options determines whether curl verifies the authenticity of the peer's certificate */

#ifdef WIN32
  curl_easy_setopt(p_curl, CURLOPT_CAINFO, "curl-bundle-ca.crt");
#endif

  curl_easy_setopt(p_curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(p_curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, custom_writer);
  curl_easy_setopt(p_curl, CURLOPT_WRITEDATA, psz_response);
  
  res = (int) curl_easy_perform(p_curl);
  if(res != 0) {
    debug(__LINE__, "Curl failed with code %d", res);
    res = FAILURE;
    goto exit;
  }

  debug(__LINE__, "[Authy] Curl response: Body=%s\n", psz_response);
  res = SUCCESS;
exit:
  /* Always cleanup otherwise it will lead to memoryleaks */
#ifdef WIN32
  debug(__LINE__, "[Authy] Can't clean curl, curl easy cleanup doesn't work on windows");
#else
  curl_easy_cleanup(p_curl);
#endif

  return res;
}

/*
 * Enable two-factor on a specific user
 * it sends a POST to https://api.authy.com/protected/{FORMAT}/users/new?api_key={KEY}
 *
 * returns:
 *   SUCCESS: if the user was added correctly.
 *   FAILURE: failed to add user.
 */
extern int
register_user(const char *psz_API_url, const char *psz_API_key, char *psz_post_fields, char *psz_response)
{
  int res = FAILURE;
  char *psz_url = NULL;
  char *psz_end_point = "/users/new";
  char *psz_params = "?api_key=";

  psz_url = url_builder(psz_API_url, psz_API_key, psz_end_point, psz_params);

  if(psz_url == NULL) {
    goto error;
  }

  res = request(psz_url, psz_post_fields, psz_response);

  psz_url = clean_and_free(psz_url);

  return res;

error:
  return FAILURE;
}

/*
 * verify
 * verifies a token
 * it sends a GET to https://api.authy.com/protected/{FORMAT}/verify/{TOKEN}/{AUTHY_ID}?api_key={KEY}
 */
extern int
verify(const char *psz_API_url, const char *psz_API_key, char *psz_token, char *psz_authy_ID, char *psz_response)
{
  int res = FAILURE;
  size_t end_point_size = 0;
  char *psz_url = NULL;
  char *psz_end_point = NULL;
  char *psz_params = "?force=true&api_key=";

  end_point_size = strlen("/verify/") + strlen(psz_token) + strlen("/") + strlen(psz_authy_ID) + 1;
  psz_end_point = calloc(end_point_size, sizeof(char));
  if(!psz_end_point){
    res = FAILURE;
    goto exit;
  }

  snprintf(psz_end_point, end_point_size, "/verify/%s/%s", psz_token, psz_authy_ID);
  psz_url = url_builder(psz_API_url, psz_API_key, psz_end_point, psz_params);

  if(!psz_url) {
    res = FAILURE;
    goto exit;
  }

  res = request(psz_url, NULL, psz_response);


exit:
  psz_url = clean_and_free(psz_url);
  psz_end_point = clean_and_free(psz_end_point);

  return res;
}

/*
 * request sms
 * it requests for sms tokens
 * sends a GET to https://api.authy.com/protected/{FORMAT}/sms/{AUTHY_ID}?api_key={KEY}
 */
extern int
request_sms(const char *psz_API_url, const char *psz_API_key, char *psz_authy_ID, char *psz_response)
{
  int res = FAILURE;
  size_t end_point_size = 0;
  char *psz_url = NULL;
  char *psz_end_point = NULL;
  char *psz_params = "?api_key=";

  end_point_size = strlen("/sms/") + strlen(psz_authy_ID) + 1;
  psz_end_point = calloc(end_point_size, sizeof(char));
  if(!psz_end_point){
    goto exit;
  }
  snprintf(psz_end_point, end_point_size, "/sms/%s", psz_authy_ID);

  psz_url = url_builder(psz_API_url, psz_API_key, psz_end_point, psz_params);
  if(!psz_url) {
    res = FAILURE;
    goto exit;
  }

  res = request(psz_url, NULL, psz_response);

exit:
  psz_url = clean_and_free(psz_url);
  psz_end_point = clean_and_free(psz_end_point);

  return res;
}
