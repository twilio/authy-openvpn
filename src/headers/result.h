//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   result.h
//
// Abstract
// 
// Wraps a result object used throughout the project to return function status.
//
//
//
// History
//
//  8/1/2013    dpalacio    Created
//
//------------------------------------------------------------------------------

#ifndef __RESULT_H__
#define __RESULT_H__ 1

#define FAILED(r) (((RESULT)(r)) != 0) // Test if result is different than 0
#define SUCCESS(r)(((RESULT)(r)) == 0) //test if result is OK

typedef enum 
{
	FAIL = -1,
	OK = 0,
	INVALID_ARG = -2,
	OUT_OF_MEMORY = -3
} RESULT;

#endif
