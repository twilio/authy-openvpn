//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   authy_conf.c
//
// Abstract
// 
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------
#include <assert.h>

#include "authy_conf.h"
#include "logger.h"
#include "utils.h"
#include "constants.h"

#define LINE_LENGTH 80 // CONF FILE MAX LINE LENGTH 

#ifdef WIN32
#define snprintf _snprintf
#endif

// Description
//
// Checks if pszAuthyId is a valid authyId
// by checking if it's a number only.
//
// Parameters
// 
//   pszAuthyId - The Authy ID
// 
// Returns
// 
// 1 if it's a valid authy ID 
// 0 otherwie
//
static BOOL
isAnAuthyId(char *pszAuthyId)
{
  long long llAuthyId =  (long long)strtoll(pszAuthyId, (char **)NULL, 10);
  if( 0 == llAuthyId){
    trace(DEBUG, __LINE__, "%s is not an Authy ID\n", pszAuthyId);
    return 0;
  }
  return 1;
}

// Description
//
// Checks that the username is valid
//
// Parameters
// 
//   pszUsername - The username to test 
//
// Returns
// 
// Standard RESULT 
//
static RESULT 
checkUsername(const char *pszUsername)
{
  RESULT r = FAIL;
  char *pszTempUsername = strdup(pszUsername);
  if(NULL == pszTempUsername){
    r = OUT_OF_MEMORY;
    trace(ERROR, __LINE__, "[Authy] Out of memory\n");
    goto EXIT;
  }
  
  removeSpaces(pszTempUsername);

  if(strlen(pszTempUsername) <= 0){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Invalid username %s=\n", pszUsername);
    goto EXIT;
  }
  
  r = OK;

EXIT:
  cleanAndFree(pszTempUsername);
  pszTempUsername = NULL;
  return r;
}


// Description
//
// Extracts the Authy ID from the configuration file 
//
// Parameters
// 
//   pszConfFilename - Full path to the configuration file
//   pszUsername     - The Username (login) for which we are getting the Authy ID
//   pszCommonName   - Common name from the OpenVPN certificate
//
// Returns
// 
// standard RESULT
//
RESULT
getAuthyIdAndValidateCommonName(__out char *pszAuthyId,
           const char *pszConfFilename, 
           const char *pszUsername, 
           const char *pszCommonName) 
{
  FILE *fpConfFile = NULL;
	char *pch = NULL;
  RESULT r = FAIL;

  char line[LINE_LENGTH];
  char *columns[3] = {NULL};
  int i = 0;


  if(!pszConfFilename || !pszUsername || FAILED(checkUsername(pszUsername))) {
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] getAuthyId: Wrong configuration file or username\n");
    goto EXIT;
  }

  fpConfFile = fopen(pszConfFilename, "r");

  if(NULL == fpConfFile) {
		trace(ERROR, __LINE__, "[Authy] getAuthyId: unable to read file %s\n", pszConfFilename);
    r = FAIL;
    goto EXIT;
  }

  memset(columns, 0, sizeof(columns));
  while(NULL != fgets(line, ARRAY_SIZE(line), fpConfFile)){
		pch = strtok(line," \t");
    i = 0;
    while(pch != NULL && i < 3){
      columns[i] = removeSpaces(pch);
      pch = strtok (NULL, " \t"); //Go to the next token 
      i++;
    }    
    if(columns[1] != NULL && 0 == strcmp(columns[0], pszUsername)){
      trace(DEBUG, __LINE__, "[Authy] Found column for pszUsername=%s column is %s\n", pszUsername, line); 
      break; 
    }
    memset(columns, 0, sizeof(columns));
  }

  if(columns[0] == NULL){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Username %s not found in Authy Config file\n", pszUsername);
    goto EXIT;
  } 
  
  assert(columns[1] != NULL);
  
  if(FALSE == isAnAuthyId(columns[1])){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] AuthyID %s for Username %s is not valid. Authy ID's can only be numeric values\n", columns[1], pszUsername);
    goto EXIT;
  }

  if(columns[2] != '\0' && strcmp(columns[2], pszCommonName) != 0){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] CommonName %s does not match the configuration file common name %s\n", pszCommonName[1], columns[2]);
    goto EXIT;
  }

  snprintf(pszAuthyId, MAX_AUTHY_ID_LENGTH, "%s", columns[1]);
  trace(INFO, __LINE__, "[Authy] Found Authy ID: %s for username: %s\n", pszAuthyId, pszUsername);
  r = OK;

EXIT:
  if(fpConfFile){
    fclose(fpConfFile);
  }
  return r;
}
