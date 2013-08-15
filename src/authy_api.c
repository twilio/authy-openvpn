#include <stdarg.h>

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
  memset(p_target, 0, sizeof p_target);
  free(p_target);
  return NULL;
}

/*
 * url size
 * calls the size need for an api url
 */
static int
url_size(const char *psz_API_url, const char *psz_end_point, const char *psz_API_key)
{
  return strlen(psz_API_url) + strlen(psz_end_point) + strlen("?api_key=") + strlen(psz_API_key) + 1;
}

/* url builder
 * builds the url and save it to psz_url
 * and returns the actual size of the url
 */
static int
url_builder(const char *psz_API_url, const char *psz_API_key, char *psz_end_point, char *psz_url, size_t size_url)
{
  return snprintf(psz_url, size_url, "%s%s?api_key=%s", psz_API_url, psz_end_point, psz_API_key);
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
 */
static int
request(char *psz_url, char *psz_post_fields, char *psz_response)
{
  CURL *p_curl;
  int i_res;

  curl_global_init(CURL_GLOBAL_ALL);

  p_curl = curl_easy_init();

  i_res = FAILURE;

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
  
  i_res = (int) curl_easy_perform(p_curl);
  debug(__LINE__, "curl response code %d, response %s\n", i_res, psz_response);
exit:
  /* Always cleanup otherwise it will lead to memoryleaks */
#ifdef WIN32
  debug(__LINE__, "Can't clean curl, curl easy cleanup doesn't work on windows");
#else
  curl_easy_cleanup(p_curl);
#endif

  return i_res;
}

/*
 * register user
 * enable two-factor on a specific user
 * it sends a POST to https://api.authy.com/protected/{FORMAT}/users/new?api_key={KEY}
 */
extern int
register_user(const char *psz_API_url, const char *psz_API_key, char *psz_post_fields, char *psz_response)
{
  int i_res = FAILURE;
  size_t size_url = 0;
  char *psz_url = NULL, *psz_end_point = "/users/new";

  size_url = url_size(psz_API_url, psz_end_point, psz_API_key);
  psz_url = (char *) calloc(size_url, sizeof(char));
  if(!psz_url){
    goto exit;
  }

  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url, size_url);

  i_res = request(psz_url, psz_post_fields, psz_response);

exit:
  psz_url = clean_and_free(psz_url);

  return i_res;
}

/*
 * verify
 * verifies a token
 * it sends a GET to https://api.authy.com/protected/{FORMAT}/verify/{TOKEN}/{AUTHY_ID}?api_key={KEY}
 */
extern int
verify(const char *psz_API_url, const char *psz_API_key, char *psz_token, char *psz_authy_ID, char *psz_response)
{
  int i_res = FAILURE;
  size_t size_url = 0, size_end_point = 0;
  char *psz_url = NULL, *psz_end_point = NULL;

  size_end_point = strlen("/verify/") + strlen(psz_token) + strlen("/") + strlen(psz_authy_ID) + 1;
  psz_end_point = (char *) calloc(size_end_point, sizeof(char));
  if(!psz_end_point){
    goto exit;
  }

  snprintf(psz_end_point, size_end_point, "/verify/%s/%s", psz_token, psz_authy_ID);

  size_url = url_size(psz_API_url, psz_end_point, psz_API_key);
  psz_url = (char *) calloc(size_url, sizeof(char));

  if(!psz_url)
    goto exit;

  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url, size_url);

  i_res = request(psz_url, NULL, psz_response);

exit:
  psz_url = clean_and_free(psz_url);
  psz_end_point = clean_and_free(psz_end_point);

  return i_res;
}

/*
 * request sms
 * it requests for sms tokens
 * sends a GET to https://api.authy.com/protected/{FORMAT}/sms/{AUTHY_ID}?api_key={KEY}
 */
extern int
request_sms(const char *psz_API_url, const char *psz_API_key, char *psz_authy_ID, char *psz_response)
{
  int i_res = FAILURE;
  size_t size_url = 0, size_end_point = 0;
  char *psz_url = NULL, *psz_end_point = NULL;

  size_end_point = strlen("/sms/") + strlen(psz_authy_ID) + 1;
  psz_end_point = (char *) calloc(size_end_point, sizeof(char));
  if(!psz_end_point){
    goto exit;
  }
  snprintf(psz_end_point, size_end_point, "/sms/%s", psz_authy_ID);

  size_url = url_size(psz_API_url, psz_end_point, psz_API_key);
  psz_url = (char *) calloc(size_url, sizeof(char));

  if(!psz_url){
    goto exit;
  }
  url_builder(psz_API_url, psz_API_key, psz_end_point, psz_url, size_url);

  i_res = request(psz_url, NULL, psz_response);

exit:
  psz_url = clean_and_free(psz_url);
  psz_end_point = clean_and_free(psz_end_point);

  return i_res;
}
