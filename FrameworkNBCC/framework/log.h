/****************************************************************************\
|* PS2 application framework
|* =========================
|* 
|* This is a simple logging facility - allows the coder to monitor the program
|* without having to use alt-X on the keyboard...
|*
|* Author				: gorobei
|* First created		: Tue Jul  9 15:56:09 GMT 2002
|* Modification history : See below
|*
\****************************************************************************/

#ifndef __logger__header__
#define __logger__header__

#include <stdio.h>
#include <stdarg.h>

class Log
	{
	protected:
		FILE *				_fd;			// The file to log to
		
	public:
							Log();
							~Log();
							
  	Log &					operator << ( const char *v );
  	Log &					operator << ( int v );
  	Log &					operator << ( unsigned int v );
  	Log &					operator << ( double v );
	
	void					printf(char *fmt, ...);
	};

extern Log _log;

#endif // ! __logger__header__
