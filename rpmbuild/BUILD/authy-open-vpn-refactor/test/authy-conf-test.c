#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define SUCCESS 0
#define FAILURE 1

void
test(int expected, const char *psz_username,
     const char *psz_common_name, char *expected_authy_id,
     const char *msg)
{
  static int kase = 0;
  printf("Test Case #%d\n", ++kase);
  char authy_id[8];
  int r = get_authy_ID("sample.conf", psz_username, psz_common_name,
                       authy_id);
  if(r != expected)
    {
      puts(msg);
      return;
    }
  if(r == 0 && strcmp(expected_authy_id, authy_id) != 0)
    {
      puts(msg);
      return;
    }
  puts("runs as expected");
}

int main()
{

  test(1, "\t\r\r\t", "\r\r\t", "so", "this just need to fail");
  
  test(1, NULL, NULL, NULL, "this must be an error if tries to check\
  for NULL user");

  test(1, NULL, NULL, NULL, "this must be an error if tries to check\
  for NULL user");

  test(1, NULL, NULL, NULL, "this must be an error if tries to check\
  for NULL user");

  test(1, "", NULL, NULL, "this must be an error if check for the\
  blank user \"\"");

  test(1, " ", NULL, NULL, "this must be an error if check for the\
  blank user \" \"");

  test(1, "      ", NULL, NULL, "this must be an error if check for the\
  blank user \"      \"");

  test(1, "\t\n", NULL, NULL, "this must be an error if check for the\
  blank user \"\\t\\n\"");

  test(1, "\r\n", NULL, NULL, "this must be an error if check for the\
  blank user \"\\r\\n\"");

  test(1, "david", "", "", "this must be an error because we don't\
  have a david in our configuration file");

  test(1, "sarcilav", "", "", "this must be an error because sarcilav\
  is using a common name in the configuration file");

  test(0, "sarcilav", "sebastian", "0000013", "this is the line\
  sarcilav sebastian 0000013");

  test(0, "sarciav", "cualquiercosa", "0000012", "this is the line\
  sarciav 0000012");

  test(0, "daniel", "daniel", "0000001", "this is the line daniel\
  daniel 0000001");

  test(1, "daniel", "sdaniel", "", "this must be an error because the\
  common name check fails");

  return 0;
}
