#include "vsprintf.h"

char output[1024] = {'\0'};
char input[1024]  = {'\0'};

int printf(const char *fmt, ...)
{
	unsigned int i = 0;
	va_list args;
	//1. �����ת��Ϊ�ַ���
	va_start(args, fmt); //fmtת��Ϊ����б�
	vsprintf((char*)output, fmt, args);// ����б�ת��Ϊ�ַ���
	va_end();//ת������

	//2.��ӡ�ַ�������
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

	//1. ��ȡ������ַ���
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

	//2. ��ʽת��
	va_start(args, fmt);
	vsscanf((char*)input, fmt, args);
	va_end();

	return (i);
}
