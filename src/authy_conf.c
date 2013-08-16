#include "authy-conf.h"

#define LINE_LENGTH 80

#ifdef WIN32
#define snprintf _snprintf
#endif

/*
 * check authy id
 * checks if psz_authy_id is an authy_id
 * by doble checking if it is an number
 */
static int
check_authy_id(char *psz_authy_id)
{
  int i_authy_id = atoi(psz_authy_id);
  char sz_authy_id_from_i[20];
  snprintf(sz_authy_id_from_i, 20, "%d", i_authy_id);
  return strncmp(psz_authy_id, sz_authy_id_from_i, 20);
}

/*
 * remove spaces
 * cleans strings from whitespaces
 */
static void remove_spaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != '\0')
  {
    *i = *j++;
    if(*i != ' ' && *i != '\n' && *i != '\r' && *i != '\t'){
      i++;
    }
  }
  *i = '\0';
}

/*
 * check username
 * avoid to match usernames with whitespaces
 */
static int check_username(const char *psz_username)
{
  int i_status = FAILURE;
  char *psz_tmp_username = strdup(psz_username);
  remove_spaces(psz_tmp_username);

  if(strlen(psz_tmp_username) > 0){
    i_status = SUCCESS;
  }
  return i_status;
}

/*
 * get authy ID
 * traverse a config file that follows one of following line patterns
 * USERNAME1 AUTHY_ID1
 * USERNAME2 COMMO_NAME2 AUTHY_ID2
 * looking for the authy_ID
 */
int
get_authy_ID(const char *psz_conf_file_name, const char *psz_username, const char *psz_common_name, char *psz_authy_id)
{
  int i_status = FAILURE;
  FILE *p_conf_file = NULL;

  if(!psz_conf_file_name || !psz_username || check_username(psz_username) == FAILURE) {
    return i_status;
  }

  p_conf_file = fopen(psz_conf_file_name, "r");

  if(!p_conf_file) {
    i_status = FAILURE;
    goto exit;
  }

  char format_we_need[LINE_LENGTH];
  snprintf(format_we_need, LINE_LENGTH, "%s %%20s", psz_username);

  char line[LINE_LENGTH];
  memset(psz_authy_id, 0, sizeof psz_authy_id);

  int b_format_found = 0;

  /* Traverse the config file until we find the line that matches the
     format: USERNAME STRING*/
  do {
    if(NULL == fgets(line, LINE_LENGTH, p_conf_file)){
      break;
    }
    b_format_found |= sscanf(line, format_we_need, psz_authy_id);
  } while(!b_format_found);

  /* they query was made for a not listed user */
  if(!b_format_found){
    i_status = FAILURE;
    goto exit;
  }

  // Check if second value is really an authy_id
  if(check_authy_id(psz_authy_id))
  {
    /*
      This means that we have to take care of the common_name
      and psz_authy_id is pointing to the common_name value.
    */
    snprintf(format_we_need, LINE_LENGTH, "%s %s %%20s", psz_username, psz_common_name);

    sscanf(line, format_we_need, psz_authy_id);

    if(check_authy_id(psz_authy_id)) {
      // the username and common name doesn't match, gg
      i_status = FAILURE;
      goto exit;
    }
  }

  i_status = SUCCESS;

exit:
  if(p_conf_file){
    fclose(p_conf_file);
  }
  return i_status;
}
