/*------------------------------------------------------------------
 * so_log.cxx - the iHi client logger.
 *
 * Mar. 2015, Cai Wenfeng
 *
 * Copyright (c) 2015 by Calidus, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 */
 
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "so_log.h"

bool iHiLog::bLogFile;
int iHiLog::nLogLevel;
pthread_mutex_t 	iHiLog::sMutex;

#define XPRINT() \
	va_start ( arg_ptr, format ) ; \
	int count = vsnprintf (buffer + 28, 2048, format, arg_ptr) ; \
	va_end ( arg_ptr ) ; \
	\
	if ( count < 0 ) \
	{ \
		return ; \
	} \
	\
	buffer[count+28] = '\0'; \	
	write(buffer)

void iHiLog::init()
{
	iHiLog::bLogFile = true; 
	iHiLog::nLogLevel = 0;
	pthread_mutex_init ( &iHiLog::sMutex, NULL ) ;
}

void iHiLog::uninit()
{
	pthread_mutex_destroy ( &iHiLog::sMutex ) ;
}

void iHiLog::debug(const char * format, ...)
{
	if ( nLogLevel > 0 )
	{
		return ;
	}
	
	char		buffer[4096] = {0};
	va_list 	arg_ptr ;
	time_t 	nowtime = time(NULL);	
	tm * 	now = localtime(&nowtime);

	memset ( buffer, 0, 4096 ) ;
	sprintf ( buffer, "%04d-%02d-%02d %02d:%02d:%02d [Debug] ", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday, 
		now->tm_hour, now->tm_min, now->tm_sec ) ;

	XPRINT();	
}

void iHiLog::info(const char * format, ...)
{
	if ( nLogLevel > 1 )
	{
		return ;
	}
	
	char		buffer[4096] = {0};
	va_list 	arg_ptr ;
	time_t 	nowtime = time(NULL);	
	tm * 	now = localtime(&nowtime);

	memset ( buffer, 0, 4096 ) ;
	sprintf ( buffer, "%04d-%02d-%02d %02d:%02d:%02d [Info ] ", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday, 
		now->tm_hour, now->tm_min, now->tm_sec ) ;
	
	XPRINT();
}

void iHiLog::warn(const char * format, ...)
{
	if ( nLogLevel > 2 )
	{
		return ;
	}
		
	char		buffer[4096] = {0};
	va_list 	arg_ptr ;
	time_t 	nowtime = time(NULL);	
	tm * 	now = localtime(&nowtime);

	memset ( buffer, 0, 4096 ) ;
	sprintf ( buffer, "%04d-%02d-%02d %02d:%02d:%02d [Warn ] ", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday, 
		now->tm_hour, now->tm_min, now->tm_sec ) ;
	
	XPRINT();
}

void iHiLog::error(const char * format, ...)
{
	if ( nLogLevel > 3 )
	{
		return ;
	}
		
	char		buffer[4096] = {0};
	va_list 	arg_ptr ;
	time_t 	nowtime = time(NULL);	
	tm * 	now = localtime(&nowtime);

	memset ( buffer, 0, 4096 ) ;
	sprintf ( buffer, "%04d-%02d-%02d %02d:%02d:%02d [Error] ", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday, 
		now->tm_hour, now->tm_min, now->tm_sec ) ;
	
	XPRINT();
}

void iHiLog::fatal(const char * format, ...)
{
	if ( nLogLevel > 4 )
	{
		return ;
	}
		
	char		buffer[4096] = {0};
	va_list 	arg_ptr ;
	time_t 	nowtime = time(NULL);	
	tm * 	now = localtime(&nowtime);

	memset ( buffer, 0, 4096 ) ;
	sprintf ( buffer, "%04d-%02d-%02d %02d:%02d:%02d [Fatal] ", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday, 
		now->tm_hour, now->tm_min, now->tm_sec ) ;
	
	XPRINT();
}

void iHiLog::write(const char * buffer)
{
	printf("%s", buffer);
	printf("\n");

	if ( bLogFile == false )
	{
		return ;
	}
	
	pthread_mutex_lock( &iHiLog::sMutex ) ;
	FILE * fd = fopen ( "iHi-endpoint.log", "a+");
	if ( fd != NULL )
	{
		fwrite(buffer, 1, strlen(buffer), fd);
		fwrite("\n", 1, 1, fd);
		fclose ( fd );
	}
	pthread_mutex_unlock( &iHiLog::sMutex ) ;
}

