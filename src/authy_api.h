#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern int register_user(const char *pszAPIUrl, const char *pszAPIKey,
                         char *pszPostFields, char *pszResponse);

extern int verify(const char *pszAPIUrl, const char *pszAPIKey,
                  char *pszToken, char *pszAuthyID, char *pszResponse);

extern int request_sms(const char *pszAPIUrl, const char *pszAPIKey,
                       char *pszAuthyID, char *pszResponse);
