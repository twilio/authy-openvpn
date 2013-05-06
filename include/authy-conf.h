#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

#define AUTHYTOKENSIZE 7

int get_authy_ID(const char *psz_conf_file_name, const char
                 *psz_username, const char *psz_common_name, char *psz_authy_id);
