#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern int
register_user(const char *psz_API_url, const char *psz_API_key, char *psz_post_fields, char *psz_response);

extern int
verify(const char *psz_API_url, const char *psz_API_key, char *psz_token, char *psz_authy_ID, char *psz_response);

extern int
request_sms(const char *psz_API_url, const char *psz_API_key, char *psz_authy_ID, char *psz_response);
