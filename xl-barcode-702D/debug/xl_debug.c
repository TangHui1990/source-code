#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>

#include "xl_debug.h"

unsigned int g_u32OutputStart = 0;
char g_szOutput[MAX_OUTPUT_COUNT][512];
static pthread_mutex_t g_tTraceLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char g_u8TraceIndex = 0;
void XL_Trace(const char* _szColor, const char* _szPrefix, const char* _szFile, unsigned int _u32Line, const char* _szFunction, const char* _szFmt, ...)
{
	pthread_mutex_lock(&g_tTraceLock);
	int s32ErrnoCpy = errno;

	char *szBasename = strrchr(_szFile, '/');
	if (szBasename == NULL)
	{
		szBasename = (char*)_szFile;
	}

	time_t timet = time(NULL);
	struct tm timetm;
	localtime_r(&timet, &timetm);

	long int threadid = syscall(224);
	if (threadid == -1)
	{
		threadid = (long int)getpid();
	}

	unsigned int u32Using;
	unsigned int u32Left;

	XL_ASSERT(g_u32OutputStart > MAX_OUTPUT_COUNT, "g_u32OutputStart %u is invalid", g_u32OutputStart);
	memset(g_szOutput[g_u32OutputStart], 0, sizeof(g_szOutput[g_u32OutputStart]));
	u32Using = strlen(g_szOutput[g_u32OutputStart]);
	u32Left = sizeof(g_szOutput[g_u32OutputStart]) - u32Using;

	snprintf(g_szOutput[g_u32OutputStart] + u32Using, u32Left, "%s%04d-%02d-%02d %02d:%02d:%02d [%02hhu] %04ld %6s %s:%s:%u "NONE,
			_szColor, timetm.tm_year+1990, timetm.tm_mon+1, timetm.tm_mday, timetm.tm_hour, timetm.tm_min, timetm.tm_sec, g_u8TraceIndex, threadid, _szPrefix, szBasename, _szFunction, _u32Line);
	
	u32Using = strlen(g_szOutput[g_u32OutputStart]);
	u32Left = sizeof(g_szOutput[g_u32OutputStart]) - u32Using;

	va_list argptr;
	va_start(argptr, _szFmt);
	vsnprintf(g_szOutput[g_u32OutputStart] + u32Using, u32Left, _szFmt, argptr);
	va_end(argptr);

	u32Using = strlen(g_szOutput[g_u32OutputStart]);
	u32Left = sizeof(g_szOutput[g_u32OutputStart]) - u32Using;
	
	g_szOutput[g_u32OutputStart][u32Using] = '\n';
	g_szOutput[g_u32OutputStart][sizeof(g_szOutput[0]) - 2] = '\n';
	g_szOutput[g_u32OutputStart][sizeof(g_szOutput[0]) - 1] = 0;
	printf("%s", g_szOutput[g_u32OutputStart]);

	g_u32OutputStart++;
	g_u32OutputStart %= MAX_OUTPUT_COUNT;
	g_u8TraceIndex = (g_u8TraceIndex < 99) ? (g_u8TraceIndex+1) : 0;

	errno = s32ErrnoCpy;
	pthread_mutex_unlock(&g_tTraceLock);
}



