#include <stdarg.h>
//------------------------------------------------------------------------------
//
// Copyright (c) Authy Inc.
//
// Name
//
//   logger.c
//
// Abstract
// 
// Implements a re-usable logger
// 
// registerUser
// verifyToken
// requestSMS
// 
//
//
// History
//
//  8/2/2013    dpalacio    Created
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdarg.h>


//
// Description
//
// Prints a debug message on the openVPN log.
// Include the line number if debug is set.
//
// Parameters
// 
//   line - The line number
//   msg - C formatted string with debug message 
// 
// Returns
// void
//

#ifdef DDEBUG
  static int g_logLevel = 5;
#elif defined DERROR
  static int g_logLevel = 3;
#else
  static int g_logLevel = 4;
#endif

void 
trace(const int level, const int line, const char *format, ...)
{
  if(level <= g_logLevel)
	{
		va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end (arg);
		fflush(stderr);
	}
}


