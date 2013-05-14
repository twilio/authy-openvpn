#include "authy-conf.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

static int
recheck_authy_id(char *psz_authy_id)
{
  int i_authy_id = atoi(psz_authy_id);
  char sz_authy_id_from_i[20];
  snprintf(sz_authy_id_from_i, 20, "%d", i_authy_id);
  return strcmp(psz_authy_id, sz_authy_id_from_i);
}

static void remove_spaces(char* source)
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

static int check_username(const char *psz_username)
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

  sprintf(format, "%s %%20s", psz_username);

  char line[80];
  memset(psz_authy_id, 0, sizeof psz_authy_id);

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
      sprintf(format, "%s %s %%20s", psz_username, psz_common_name);
      sscanf(line, format, psz_authy_id);

      if(recheck_authy_id(psz_authy_id))
        /* the username and common name doesn't match, gg */
        goto exit;
    }

  i_status = SUCCESS;

 exit:
  fclose(p_conf_file);
  return i_status;
}
