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
#include <errno.h>

#include "authy_conf.h"
#include "logger.h"
#include "utils.h"
#include "constants.h"

#define ALLOW_NON_TWO_FACTOR_LOGINS TRUE

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
// Validates that the given common name in the certificate
// matches the common name for the user in the config file.
// 
// Parameters
// 
//   pszCommonName  - The common name on the certificate. 
//   pszWantedCommonName - The common name on the config file 
//
// Returns
// 
// Standard RESULT
//
RESULT
validateCommonName(const char *pszCommonName, 
                   const char *pszWantedCommonName
                  )
{

  RESULT r = FAIL;

  if(pszCommonName != NULL && strcmp(pszCommonName, pszWantedCommonName) == 0){
    r = OK;
    trace(ERROR, __LINE__, "[Authy] CommonName validation succeeded\n");
  }
  else{
    trace(ERROR, __LINE__, "[Authy] CommonName %s does not match the configuration file common name %s\n", pszCommonName, pszWantedCommonName);
  }

  return r;
}

// Description
//
// Extracts the Authy ID and common name from the configuration file 
//
// Parameters
//    
//   ppszAuthyId     - A pointer to the pointer that will hold the authy id string
//   ppszCommonName  - A pointer to the pointer that will hold the CommonName String 
//   pszConfFilename - Full path to the configuration file
//   pszUsername     - The Username (login) for which we are getting the Authy ID
//   pszCommonName   - Common name from the OpenVPN certificate
//
// Returns
// 
// standard RESULT
//
RESULT
getAuthyIdAndCommonName(__out char **ppszAuthyId,
                        __out char **ppszCommonName,
                        const char *pszConfFilename, 
                        const char *pszUsername 
                        ) 
{
  FILE *fpConfFile = NULL;
	char *pch = NULL;

  char line[LINE_LENGTH];
  char *columns[3] = {NULL};
  int i = 0;

  RESULT r = FAIL;

  if(!pszConfFilename){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Wrong configuration filename.\n");
    goto EXIT;
  }

  if(!pszUsername || FAILED(checkUsername(pszUsername)) ) {
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Username is invalid. Either empty or not a valid unix username.\n");
    goto EXIT;
  }

  fpConfFile = fopen(pszConfFilename, "r");

  if(NULL == fpConfFile) {
		trace(ERROR, __LINE__, "[Authy] Unable to read authy config file: %s.\n", pszConfFilename);
    trace(ERROR, __LINE__, "[Authy] Error %d: %s\n", errno, strerror(errno));
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
    if(0 == strcmp(columns[0], pszUsername)){
      trace(DEBUG, __LINE__, "[Authy] Found column for pszUsername=%s column is %s\n", pszUsername, line); 
      break; 
    }
    memset(columns, 0, sizeof(columns));
  }

  if(columns[0] == NULL){
    r = FAIL;
    if (ALLOW_NON_TWO_FACTOR_LOGINS){
      r = OK;
    }
    trace(ERROR, __LINE__, "[Authy] Username %s was not found in Authy Config file.\n", pszUsername);
    goto EXIT;
  } 
  
  if(columns[1] == NULL || isAnAuthyId(columns[1])== FALSE ){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] AuthyID %s for Username %s is not valid. Authy ID's can only be numeric values\n", columns[1], pszUsername);
    goto EXIT;
  }
  
  trace(INFO, __LINE__, "[Authy] Found Authy ID: %s for username: %s\n", columns[1], pszUsername);

  *ppszAuthyId = strdup(columns[1]);
  if(!*ppszAuthyId){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Unable to allocate memory to copy AuthyID.\n");
    goto EXIT;
  }

  
  if(columns[2]){
    *ppszCommonName = strdup(columns[2]);
    if(!*ppszCommonName){
      trace(ERROR, __LINE__, "[Authy] Unable to allocate memory to copy CommonName.\n");
    }
  }

  r = OK;

EXIT:
  if(fpConfFile){
    fclose(fpConfFile);
  }
  return r;
}
