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

#include "custom_types.h"
#include "utils.h"

RESULT
getAuthyIdAndCommonName(__out char **ppszAuthyId,
                        __out char **ppszCommonName,
                        const char *pszConfFilename, 
                        const char *pszUsername 
                        );

RESULT
validateCommonName(const char *pszCommonName, 
                   const char *pszWantedCommonName);
#endif
