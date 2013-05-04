#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define SUCCESS 0
#define FAILURE 1

int
recheck_authy_id(char *psz_authy_id)
{
  int i_authy_id = atoi(psz_authy_id);
  char sz_authy_id_from_i[8];
  snprintf(sz_authy_id_from_i, 8, "%.7d", i_authy_id);
  return strcmp(psz_authy_id, sz_authy_id_from_i);
}

void remove_spaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != '\0')
    {
      *i = *j++;
      if(*i != ' ' && *i != '\n' && *i != '\r' && *i != '\t')
        i++;
    }
  *i = '\0';
}

int check_username(const char *psz_username)
{
  int i_status = FAILURE;
  char *psz_tmp_username = strdup(psz_username);
  remove_spaces(psz_tmp_username);

  if(strlen(psz_tmp_username) > 0)
    i_status = SUCCESS;

  return i_status;
}

int
get_authy_ID(const char *psz_conf_file_name, const char *psz_username,
             const char *psz_common_name, char *psz_authy_id)
{
  int i_status = FAILURE;

  if(!psz_conf_file_name || !psz_username ||
     check_username(psz_username) == FAILURE)
    return i_status;

  FILE *p_conf_file = fopen(psz_conf_file_name, "r");

  char format[80];

  sprintf(format, "%s %%s", psz_username);

  char line[80];
  memset(psz_authy_id, 0, 8);

  /* find the username line in the config file*/
  int b_found_username = 0;
  while(b_found_username == 0 && fgets(line, 80, p_conf_file))
    b_found_username |= sscanf(line, format, psz_authy_id);

  if(!b_found_username)
    /* they query was made for a not listed user */
    goto exit;

  if(recheck_authy_id(psz_authy_id))
    /*
      this means that we have to take care of the common_name
      and psz_authy_id is pointing to the common_name value
    */
    {
      if(strcmp(psz_common_name, psz_authy_id))
        /* the common name doesn't match, gg */
        goto exit;

      sprintf(format, "%s %s %%s", psz_username, psz_common_name);

      sscanf(line, format, psz_authy_id);
    }

  i_status = SUCCESS;

 exit:
  fclose(p_conf_file);
  return i_status;
}

/* void */
/* test(int expected, const char *psz_username, */
/*      const char *psz_common_name, char *expected_authy_id, */
/*      const char *msg) */
/* { */
/*   static int kase = 0; */
/*   printf("Test Case #%d\n", ++kase); */
/*   char authy_id[8]; */
/*   int r = get_authy_ID("sample.conf", psz_username, psz_common_name, */
/*                        authy_id); */
/*   if(r != expected) */
/*     { */
/*       puts(msg); */
/*       return; */
/*     } */
/*   if(r == 0 && strcmp(expected_authy_id, authy_id) != 0) */
/*     { */
/*       puts(msg); */
/*       return; */
/*     } */
/*   puts("runs as expected"); */
/* } */

/* int main() */
/* { */

/*   test(1, "\t\r\r\t", "\r\r\t", "so", "this just need to fail"); */
  
/*   test(1, NULL, NULL, NULL, "this must be an error if tries to check\ */
/*   for NULL user"); */

/*   test(1, NULL, NULL, NULL, "this must be an error if tries to check\ */
/*   for NULL user"); */

/*   test(1, NULL, NULL, NULL, "this must be an error if tries to check\ */
/*   for NULL user"); */

/*   test(1, "", NULL, NULL, "this must be an error if check for the\ */
/*   blank user \"\""); */

/*   test(1, " ", NULL, NULL, "this must be an error if check for the\ */
/*   blank user \" \""); */

/*   test(1, "      ", NULL, NULL, "this must be an error if check for the\ */
/*   blank user \"      \""); */

/*   test(1, "\t\n", NULL, NULL, "this must be an error if check for the\ */
/*   blank user \"\\t\\n\""); */

/*   test(1, "\r\n", NULL, NULL, "this must be an error if check for the\ */
/*   blank user \"\\r\\n\""); */

/*   test(1, "david", "", "", "this must be an error because we don't\ */
/*   have a david in our configuration file"); */

/*   test(1, "sarcilav", "", "", "this must be an error because sarcilav\ */
/*   is using a common name in the configuration file"); */

/*   test(0, "sarcilav", "sebastian", "0000013", "this is the line\ */
/*   sarcilav sebastian 0000013"); */

/*   test(0, "sarciav", "cualquiercosa", "0000012", "this is the line\ */
/*   sarciav 0000012"); */

/*   test(0, "daniel", "daniel", "0000001", "this is the line daniel\ */
/*   daniel 0000001"); */

/*   test(1, "daniel", "sdaniel", "", "this must be an error because the\ */
/*   common name check fails"); */

/*   return 0; */
/* } */
