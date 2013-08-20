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
#include "authy_conf.h"
#include "logger.h"
#include "utils.h"

#define LINE_LENGTH 80 // CONF FILE MAX LINE LENGTH 

#ifdef WIN32
#define snprintf _snprintf
#endif

// Description
//
// Checks if pszAuthyId is a valid authyId
// by checking if it is an number.
//
// Parameters
// 
//   pAuthyId - The pointer being free'd
// 
// Returns
// 
// Standard RESULT
//
static RESULT
validateAuthyId(char *pszAuthyId)
{
  RESULT r = FAIL;
  int iAuthyId =  (int)strtol(pszAuthyId, (char **)NULL, 10);
  if( 0 == iAuthyId){
    r = FAIL;
    trace(ERROR, __LINE__, "Invalid Authy ID=%s\n", pszAuthyId);
    goto EXIT;
  }

  char szAuthyIdFromI[20];
  snprintf(szAuthyIdFromI, 20, "%d", iAuthyId);
  if (0 != strncmp(pszAuthyId, szAuthyIdFromI, 20)){
    r = FAIL;
    trace(ERROR, __LINE__, "Invalid Authy ID=%s\n", pszAuthyId);
    goto EXIT;
  }

  r = OK;

EXIT:
  return r;
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
//   pszCommonName   -
//
// Returns
// 
// standard RESULT
//
RESULT
getAuthyId(__out char *pszAuthyId,
           const char *pszConfFilename, 
           const char *pszUsername, 
           const char *pszCommonName) 
{
  RESULT r = FAIL;
  FILE *fpConfFile = NULL;

	pszAuthyId = calloc(LINE_LENGTH, sizeof(char)); // ID is goind to be max the line length
	if(NULL == pszAuthyId){
    r = OUT_OF_MEMORY;
    trace(ERROR, __LINE__, "[Authy] Out of memory. Can't allocate enough memory for Authy ID\n");
    goto EXIT;
	}

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

  char username[LINE_LENGTH];
  snprintf(username, LINE_LENGTH, "%s %%20s", pszUsername); //set the format to the userName needed

  char line[LINE_LENGTH];
  int bUsernameFound = 0; //not found

  /* Traverse the config file until we find the line that matches the
     USERNAME*/
  do {
    if(NULL == fgets(line, LINE_LENGTH, fpConfFile)){
      break;
    }
    bUsernameFound = sscanf(line, username, pszAuthyId);//0 unless matching
  } while(!bUsernameFound);

  /* they query was made for a not listed user */
  if(0 == bUsernameFound){
    r = FAIL;
    trace(ERROR, __LINE__, "[Authy] Unable to get Authy ID for username=%s\n", pszUsername);
    goto EXIT;
  }

  // Check if second value is really an authy_id
  if(validateAuthyId(pszAuthyId))
  {
    /*
      This means that we have to take care of the common_name
      and pszAuthyId is pointing to the commonName value.
    */
    snprintf(username, LINE_LENGTH, "%s %s %%20s", pszUsername, pszCommonName);

    sscanf(line, username, pszAuthyId);

    if(validateAuthyId(pszAuthyId)) {
      // the username and common name doesn't match
      r = FAIL;
      trace(ERROR, __LINE__, "[Authy] Username and CommonName do not Match\n");
      goto EXIT;
    }
  }

  r = OK;

EXIT:
  if(fpConfFile){
    fclose(fpConfFile);
  }
  return r;
}
