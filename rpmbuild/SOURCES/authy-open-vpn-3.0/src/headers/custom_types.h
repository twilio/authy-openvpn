//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
// custom_types.h
//
// Abstract
// 
// defines new types used on the Authy OpenVPN project.
//
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------

#ifndef __CUSTOM_TYPES_H__
#define __CUSTOM_TYPES_H__ 1

#define __out  //indicates that the argument will contain the result.  


#define BOOL short // Boolean type.
#define TRUE 1
#define FALSE 0


#define FAILED(r) (((RESULT)(r)) != 0) // Test if result is different than 0
#define SUCCESS(r)(((RESULT)(r)) == 0) //test if result is OK

typedef enum 
{
	FAIL = -1,
	OK = 0,
	INVALID_ARG = -2,
	OUT_OF_MEMORY = -3
} RESULT; // Standard result returned by most functions

#endif
