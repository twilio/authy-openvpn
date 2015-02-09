//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
// utils.h
//
// Abstract
//
// Utility functions
//
//
// History
//
//  8/1/2013    dpalacio    Created
//  9/2/2015    serargz     Edited
//
//------------------------------------------------------------------------------

#include <stdbool.h>

#ifndef __UTILS_H__
#define __UTILS_H__ 1

#define ARRAY_SIZE(array) (sizeof(array))/(sizeof(*(array)))

void cleanAndFree(void *pszPtr);
char* removeSpaces(char *pszString);
char* truncateAndSanitizeToken(char *pszString);

#endif
