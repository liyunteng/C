/*------------------------------------------------------------------
 * so_log.h - the iHi client logger.
 *
 * Mar. 2015, Cai Wenfeng
 *
 * Copyright (c) 2015 by Calidus, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 */
 
#ifndef __SO_LOG_H__
#define __SO_LOG_H__

#include <pthread.h>

class iHiLog
{
public:
	static void init();
	static void uninit();
	
	static void debug(const char * format, ...);
	static void info(const char * format, ...);
	static void warn(const char * format, ...);
	static void error(const char * format, ...);
	static void fatal(const char * format, ...);	

	static void write(const char * buffer);

	static bool bLogFile;
	static int nLogLevel;
	static pthread_mutex_t 	sMutex;
};

#endif // __SO_LOG_H__
