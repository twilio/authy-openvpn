//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   authy-conf.h
//
// Abstract
// 
// Handles Authy config file. Allows us to extract the Authy ID
// given a username.
//  
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------
#ifndef __AUTHY_CONF_H__
#define __AUTHY_CONF_H__ 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "result.h"
#include "utils.h"

#define AUTHY_TOKEN_SIZE 7    // The size of our mobile tokens
#define HARDWARE_TOKEN_SIZE 6 // Size of our hardware OTP's

RESULT
getAuthyId(__out char *pszAuthyId,
					 const char *pszConfFileName, 
           const char *pszUserName, 
           const char *pszCommonName);

#endif
