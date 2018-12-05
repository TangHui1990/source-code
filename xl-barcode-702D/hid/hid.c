#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <xl_debug.h>
#include <control.h>

extern CapsLockMode eCapsLockStatus;
extern CaseIgnored eCaseIgnoreStatus;
extern Language eLanguageStatus;

typedef unsigned char uchar;

#define BUF_LEN 512
int hid_fd = 0;

extern const uchar USA[256];
extern const uchar FRANCH[256-64];
extern const uchar GERMAN[256-64];
extern const uchar UK[256-64];
extern const uchar TERQ[256-64];
extern const uchar SLOVAK[256-64];
extern const uchar CZECH[256-64];
extern const uchar DANM[256-64];
extern const uchar JAP[256-64];
extern const uchar SPAN[256-64];
extern const uchar ITILY[256-64];
extern const uchar arfa[12];
extern const uchar and[25];
extern const uchar andtr[24];
extern const uchar KBtoHID[105][2];
//extern const uchar TRV[10];
extern const uchar TRV[15];

struct options {
    const char    *opt;
    unsigned char val;
};

struct Combinatorial_output
{
	const char *opt;
	unsigned char val[2];
};	

int keyboard_fill_report(char report[8], unsigned int buf)
{
	int i = 0;

	unsigned char iKb = 0;
	unsigned char funcflag = 0;

	if (eLanguageStatus == LANGUAGE_COMMON)			/* 通用 */
	{
		iKb = USA[buf];		
		funcflag = USA[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_USA)		/* 美国 */
	{
		iKb = USA[buf];		
		funcflag = USA[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_FRANCE)		/* 法国 */
	{
		iKb = FRANCH[buf];		
		funcflag = FRANCH[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_GERMANY)	/* 德国 */
	{
		iKb = GERMAN[buf];
		funcflag = GERMAN[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_BRITAIN)	/* 英国 */
	{
		iKb = UK[buf];
		funcflag = UK[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_TURKEY)		/* 土耳其 */
	{
		iKb = TERQ[buf];
		funcflag = TERQ[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_DENMARK)	/* 丹麦 */
	{
		iKb = DANM[buf];
		funcflag = DANM[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_JAPAN)		/* 日本 */
	{
		iKb = JAP[buf];
		funcflag = JAP[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_SPAIN)		/* 西班牙 */
	{
		iKb = SPAN[buf];
		funcflag = SPAN[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_ITALY)		/* 意大利 */
	{
		iKb = ITILY[buf];
		funcflag = ITILY[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_JACK)		/* 杰克 */
	{
		//iKb = ITILY[buf];
		//funcflag = ITILY[buf + 128];
	}
	else if(eLanguageStatus == LANGUAGE_SNOVUCK)	/* 斯诺伐克 */
	{
		iKb = SLOVAK[buf];
		funcflag = SLOVAK[buf + 128];
	}

	if ((eCaseIgnoreStatus == CASEIGNORED_OPEN) && (eCapsLockStatus == CAPSLOCK_PRESS))
	{
		if ( (!funcflag) && (isalpha(buf)))			//处理字母
		{
			report[0] = 0x02;
		}

		if (funcflag && !(isalpha(buf)))			//处理需要shift的字符
		{
			report[0] = 0x02;
		}
	}
	else 
	{
		if ( funcflag )
		{
			report[0] = 0x02;
		}
	}


	for (i = 0; i <sizeof(KBtoHID)/sizeof(KBtoHID[0]); i++)
	{
		if (iKb == KBtoHID[i][1])
		{
			report[2] = KBtoHID[i][0];

			break;
		}
	}
	
    return 8;
}


int Hid_Init(void)
{
	if ((hid_fd = open("/dev/hidg0", O_RDWR, 0666)) == -1) 
	{
		XL_TRACE_DEBUG("open /dev/hidg0 error!\n");

		return -1;
	}

	return (0);
}


int Hid_Exit(void)
{
	close(hid_fd);

	return (0);
}

int Hid_Output(const char *str, int cmd_len)
{
	char tBuf;
    char report[8];
	char report_bak[8];
    int to_send = 8;
    int i = 0;
	
	memset(report, 0x0, sizeof(report));
	memset(report_bak, 0x0, sizeof(report));

	while (cmd_len--)
	{
		tBuf = str[i];
		keyboard_fill_report(report, tBuf);
		
		write(hid_fd, report, to_send);

		memset(report, 0x0, sizeof(report));
		write(hid_fd, report, to_send);

		i++;
	}
	
	/* change line */
	memset(report, 0x0, sizeof(report));
	report[2] = 0x28;
	write(hid_fd, report, to_send);

	/* stop transfer */
	memset(report, 0x0, sizeof(report));

	write(hid_fd, report, to_send);
	
    return 0;
}

