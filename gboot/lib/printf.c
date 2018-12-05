#include "vsprintf.h"

char output[1024] = {'\0'};
char input[1024]  = {'\0'};

int printf(const char *fmt, ...)
{
	unsigned int i = 0;
	va_list args;
	//1. 将变参转化为字符串
	va_start(args, fmt); //fmt转化为变参列表
	vsprintf((char*)output, fmt, args);// 变参列表转化为字符串
	va_end();//转化结束

	//2.打印字符到串口
	for (i = 0; i < strlen((const char*)output); i++)
	{
		gboot_putc(output[i]);
	}

	return (i);
}

int scanf(const char *fmt, ...)
{
	unsigned int i = 0;
	va_list args;
	unsigned char ch;

	//1. 获取输入的字符串
	while ( 1 )
	{
		ch = gboot_getc();

		if ((ch == '\n') || (ch == '\r'))
		{
			input[i] = '\n';
			break;
		}

		input[i++] = ch;
	}

	//2. 格式转化
	va_start(args, fmt);
	vsscanf((char*)input, fmt, args);
	va_end();

	return (i);
}
