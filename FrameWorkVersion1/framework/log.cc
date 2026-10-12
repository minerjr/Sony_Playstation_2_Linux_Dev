/****************************************************************************\
|* PS2 application framework
|* =========================
|* 
|* This is a simple logging facility - allows the coder to monitor the program
|* without having to use alt-X on the keyboard...
|*
|* Author				: SjG, various others
|* First created		: Tue Jul  9 15:56:09 GMT 2002
|* Modification history : See below
|*
\****************************************************************************/
#include "log.h"

Log _log;

/****************************************************************************\
|* Constructor: 
\****************************************************************************/
Log::Log()
	{
	_fd = fopen( "log", "w" );
	}

/****************************************************************************\
|* Destructor: 
\****************************************************************************/
Log::~Log()
	{
	fclose(_fd);
	}


/****************************************************************************\
|* Operators for sending data to the log
\****************************************************************************/
Log &Log:: operator << ( const char *v )
	{
  	if( !_fd )
    	return *this;
  	
	fprintf( _fd, "%s", v );
  	fflush( _fd );
  	return *this;
	}

Log &Log::operator << ( int v )
	{
  	if( !_fd )
    	return *this;
  	fprintf( _fd, "%d", v );
  	fflush( _fd );
  	return *this;
	}

Log &Log::operator << ( unsigned int v )
	{
  	if( !_fd )
    	return *this;
  	fprintf( _fd, "0x%x", v );
  	fflush( _fd );
  	return *this;
	}

Log &Log::operator << ( double v )
	{
  	if( !_fd )
    	return *this;
  	fprintf( _fd, "%f", v );
  	fflush( _fd );
  	return *this;
	}


/****************************************************************************\
|* Printf() like functionality
\****************************************************************************/
void Log::printf(char *fmt, ...)
	{
	va_list ap;
	va_start(ap, fmt);
	(void) vfprintf(_fd, fmt, ap);
	va_end(ap);
	}
	
	
