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
trace(const int level, const int line, const char *msg, ...)
{
  if(level <= g_logLevel)
	{
		va_list arg;
		int done;
    va_start(arg, format);
    done = vfprintf(stderr, format, arg);
    va_end (arg);
		fflush(stderr);
	}
}


