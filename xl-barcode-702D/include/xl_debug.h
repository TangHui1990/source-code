#ifndef __XL_DEBUG_H__
#define __XL_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/stat.h>

#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

#define MAX_OUTPUT_COUNT (32)
extern unsigned int g_u32OutputStart;
extern char g_szOutput[MAX_OUTPUT_COUNT][512];
void XL_Trace(const char* _szColor, const char* _szPrefix, const char* _szFile, unsigned int _u32Line, const char* _szFunction, const char* _szFmt, ...);
#define XL_TRACE_FAILED(fmt, ...) XL_Trace(LIGHT_RED, "FAILED", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define XL_TRACE_INFO(fmt, ...) XL_Trace(YELLOW, "INFO", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define XL_TRACE_DEBUG(fmt, ...) XL_Trace(GREEN, "DEBUG", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#define XL_TRACE_ERROR(fmt, ...) XL_Trace(RED, "ERROR", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);

#define XL_ASSERT(express, fmt, ...)\
do\
{\
	if (express)\
	{\
		XL_Trace(BLUE, "ERROR", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);\
		assert(0);\
	}\
}while(0)

#define XL_RETURN(express, ret, fmt, ...)\
do\
{\
	if (express)\
	{\
		XL_TRACE_FAILED(fmt, ##__VA_ARGS__);\
		return (ret);\
	}\
}while(0)


#endif




