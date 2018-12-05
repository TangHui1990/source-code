#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <linux/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <error.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <linux/input.h>
#include <math.h>

#include <videodev2.h>
#include <HDSD.h>
#include <XLDECODE.h>
#include <control.h>
#include <upgrade.h>
#include <xl_debug.h>
#include <systemseting.h>

#include <device.h>

#include <dlfcn.h>

/* 真知码 */
typedef enum
{
	QRCODE_ONLY,			/* 只读QR码 */
	ZZCODE_ONLY,			/* 只读真知码 */
	QRZZ_BOTH,				/* QR真知都读 */
}ZzcodeContrl;


ZzcodeContrl ZzcodeStatus = QRZZ_BOTH;

void beep_work(void)
{
	int iVoiceCtrl = 0x0;
							
	if (eSoundLengthStatus == SOUNDLENGTH_SHORT)			/* 短音 */
	{
		iVoiceCtrl |= SHORTTONE_SOUND;
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_MIDDLE)		/* 中音 */
	{
		iVoiceCtrl |= MIDDLETONE_SOUND;
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_LONG)			/* 长音 */
	{
		iVoiceCtrl |= LONGTONE_SOUND;
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_USERDEFINE)	/* 自定义 */
	{
	}

	if (eAudioFrequencyStatus == AUDIOFREQUENCY_LOW)		/* 低频 */
	{
		iVoiceCtrl |= LOWFREQUENCY_SOUND;
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_INTERMEDIATE)	/* 中频 */
	{
		iVoiceCtrl |= MIDDLEFREQUENCY_SOUND;
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_HIGH)		/* 高频 */
	{
		iVoiceCtrl |= HIGHFREQUENCY_SOUND;
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_USERDEFINE)	/* 自定义 */
	{
	}
	
	if (eBellTransmitStatus == BELLTRANSMITION_OPEN)
	{		
		ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
	}
}

int XL_GC0308_QrCodeDealWith(void)
{
	int Date_Len;
	struct timeval Tval_Start;
	struct timeval Tval_End;
	int bFlagBuff = 1;
	int bFlagDecode = 0;
	int iFlagLedStatus = 0;
	int ret = -3;
	int buf_index;
	int i;
	int sum = 0;
	int flags;
	char report[8];

	unsigned int ButtonVal = 1;
	int ledFlag = 0;

	char *pData = NULL;

	char *img = NULL;

	typedef void (*call_func)(unsigned char*,  int , int , unsigned char*);
	void * libzzm_handle = NULL;
	char *errorInfo;
	call_func ZDecode_method = NULL;
		
	libzzm_handle = dlopen("libZCode.so", RTLD_LAZY);	 //mips,armxxx																							
	if (!libzzm_handle){																					
		printf("Open Error:%s.\n",dlerror());
		return -1;
	}
	
	ZDecode_method = dlsym(libzzm_handle,"ZDecode");
	errorInfo = dlerror();										
	if (errorInfo != NULL){
		printf("Dlsym Error:%s.\n",errorInfo);
		return -1;
	}
	
	int *nType = (int*)malloc(sizeof(int));
	if ( nType == NULL )
	{
		XL_TRACE_DEBUG("malloc nType error!!\n");

		return -1;
	}

	pData = (char*)malloc(10*1024);
	if (NULL == pData)
	{
		XL_TRACE_DEBUG("malloc pData error!!!\n");

		return -1;
	}

	char * T_Buffs = (char*)malloc(buffers[0].length);
	if (T_Buffs == NULL)
	{
		XL_TRACE_DEBUG("malloc error!\n");
		return -1;
	}

	char *T_BuffsTmp = (char *)malloc(buffers[0].length);
	if (T_BuffsTmp == NULL)
	{
		XL_TRACE_DEBUG("malloc error!\n");

		return -1;
	}

	char *yuv;

	img = (char*)malloc(PREVIEW_WIDTH*PREVIEW_HEIGHT);
	if (img == NULL)
	{
		printf("img malloc error!\n");
	}
		
	yuv = (char*)malloc(PREVIEW_WIDTH*PREVIEW_HEIGHT);
	if (yuv == NULL)
	{
		printf("yuv malloc error!\n");
	}

	int setcodeflag = 0;
	
	while ( 1 ) 
	{	
		Date_Len = 0;
		setcodeflag = 0;
		
		if (eInterfaceStatus == INTERFACE_HID)
		{
			if(flags = fcntl(hid_fd, F_GETFL, 0) < 0)
			{
				XL_TRACE_DEBUG("fcntl failure.");
				return -1;
			}
		
			flags |= O_NONBLOCK;
			if(fcntl(hid_fd, F_SETFL, flags) < 0)
			{
				XL_TRACE_DEBUG("fcntl failure.");
				return -1;
			}
			
			read(hid_fd, report, 8);
		
			if ((report[0] == 0x02) || (report[0] == 0x03))
			{
				eCapsLockStatus = CAPSLOCK_PRESS;
			}
			else
			{
				eCapsLockStatus = CAPSLOCK_NOPRESS;
			}
		
			flags &= ~O_NONBLOCK;
			if(fcntl(hid_fd, F_SETFL, flags) < 0)
			{
				XL_TRACE_DEBUG("fcntl failure.");
				return -1;
			}	
		}

		memset(pData, '\0', 10*1024);

		read(buttons_fd, &ButtonVal, 4);		
		if (((ButtonVal>>20) & 0x1) == 1)
		{	
			ioctl(BeepFd, JZ_LED_OFF, 0);

			ledFlag = 0;
		}
		else if ((ledFlag == 0) && (((ButtonVal>>20) & 0x1) == 0))
		{
			ioctl(BeepFd, JZ_LED_ON, 0);
			getyuv();
			clearyuv();
		}

		if (((ButtonVal>>20) & 0x1) == 0 && eScanModeStatus == SCANMODE_MANUAL && ledFlag == 0)
		{
			memcpy(T_BuffsTmp,  (unsigned char*)getyuv(), PREVIEW_WIDTH*PREVIEW_HEIGHT*2);
				
			for (i = 0; i < PREVIEW_WIDTH*PREVIEW_HEIGHT; i++)
			{
				memcpy(T_Buffs+i, T_BuffsTmp+i*2, 1);
			}
			
			clearyuv();

			memset(pData, '\0', 10*1024);

			Date_Len = XL_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, T_Buffs, nType, pData, 10*1024);
			if (Date_Len > 0)
			{
				if (strcmp(pData, "$008000") == 0)
				{
					ZzcodeStatus = QRZZ_BOTH;

					setcodeflag = 1;

					//beep_work();
					ioctl(BeepFd, BOOTPROMPT_SOUND, 0);

					ledFlag = 1;
					ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

					if (eLedTipsStatus == LEDTIPS_OPEN)
					{
						ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
					}

					continue;
				}
				else if(strcmp(pData, "$008100") == 0)
				{
					ZzcodeStatus = QRCODE_ONLY;

					setcodeflag = 1;

					//beep_work();
					ioctl(BeepFd, BOOTPROMPT_SOUND, 0);

					ledFlag = 1;
					ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

					if (eLedTipsStatus == LEDTIPS_OPEN)
					{
						ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
					}

					continue;
				}
				else if(strcmp(pData, "$008200") == 0)
				{
					ZzcodeStatus = ZZCODE_ONLY;

					setcodeflag = 1;

					//beep_work();
					ioctl(BeepFd, BOOTPROMPT_SOUND, 0);

					ledFlag = 1;
					ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

					if (eLedTipsStatus == LEDTIPS_OPEN)
					{
						ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
					}

					continue;
				}

				if ( Date_Len > 0)
				{
					if ((ZzcodeStatus == QRZZ_BOTH) || (ZzcodeStatus == QRCODE_ONLY))
					{
						beep_work();
						ledFlag = 1;
						ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

						if (eLedTipsStatus == LEDTIPS_OPEN)
						{
							ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
						}
						
						ret = Decode_DataProcessing(pData, Date_Len);
						XL_RETURN(ret < 0, -1, "Decode_DataInversion error!");
					}

				}
			}

			if ((ZzcodeStatus == QRZZ_BOTH) || (ZzcodeStatus == ZZCODE_ONLY))
			{
				if (Date_Len == 0)
				{
					memset(pData, '\0', 10*1024);
					int i,j;
					for (j=0; j<PREVIEW_HEIGHT; j++)
					for(i=0; i<PREVIEW_WIDTH; i++)
					img[j*PREVIEW_WIDTH+i] = T_Buffs[(PREVIEW_HEIGHT-1-j)*PREVIEW_WIDTH + i];

					(*ZDecode_method)(img, PREVIEW_WIDTH, PREVIEW_HEIGHT, pData);					

					while(pData[Date_Len]!='\0')
					{
						Date_Len++;
					}

					if ( Date_Len > 0)
					{
						ledFlag = 1;
						ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

						if (eLedTipsStatus == LEDTIPS_OPEN)
						{
							ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
						}

						beep_work();

						ret = Decode_DataProcessing(pData, Date_Len);
						XL_RETURN(ret < 0, -1, "Decode_DataInversion error!");

					}
				}				
			}	
		}	

	}
	
	stop_Capture();
	
	free(img);
	free(yuv);
	free(nType);
	free(pData);
	free(T_BuffsTmp);
	free(T_Buffs);
	
	close(BeepFd);	

	return (0);
}







