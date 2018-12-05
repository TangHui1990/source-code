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

	char *pData = NULL;

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

	char *T_BuffsAnthor = (char *)malloc(buffers[0].length);
	if (T_BuffsAnthor == NULL)
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
	
	while ( 1 ) 
	{	
		gettimeofday(&Tval_Start, NULL);

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


#if 0
		buf_index = readFrame();	

		if(buf_index == -1)
		{			
			//continue;
		}				
#endif

		if ( bFlagBuff )
		{
			bFlagBuff = !bFlagBuff;
			//memcpy(T_BuffsTmp, buffers[buf_index].start, buffers[buf_index].length);
			memcpy(T_BuffsTmp,  (unsigned char*)getyuv(), PREVIEW_WIDTH*PREVIEW_HEIGHT*2);
			
			for (i = 0; i < PREVIEW_WIDTH*PREVIEW_HEIGHT; i++)
			{
				memcpy(T_Buffs+i, T_BuffsTmp+i*2, 1);
			}

			//OutPutYuv(0, T_Buffs, PREVIEW_WIDTH, PREVIEW_HEIGHT);
		}
		else
		{
			bFlagBuff = !bFlagBuff;
			//memcpy(T_BuffsTmp, buffers[buf_index].start, buffers[buf_index].length);
			memcpy(T_BuffsTmp, (unsigned char*)getyuv(), PREVIEW_WIDTH*PREVIEW_HEIGHT*2);
			
			for (i = 0; i < PREVIEW_WIDTH*PREVIEW_HEIGHT; i++)
			{
				memcpy(T_BuffsAnthor+i, T_BuffsTmp+i*2, 1);
			}

			//OutPutYuv(0, T_BuffsAnthor, PREVIEW_WIDTH, PREVIEW_HEIGHT);
		}
		clearyuv();
		
		sum = MoveDetect(T_Buffs, T_BuffsAnthor, PREVIEW_WIDTH, PREVIEW_HEIGHT);
		
		XL_TRACE_DEBUG("\nsum = %d\n", sum);

		if ( sum > 35000 )
		{			
			if ( bFlagDecode )
			{
				continue;		
			}
			
			ioctl(BeepFd, JZ_LED_ON, 0);
			iFlagLedStatus = 1;
			
			if ( !bFlagBuff )
			{
				Date_Len = HD_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, T_Buffs, nType, pData, 10*1024);
			    //Date_Len = XL_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, T_Buffs, nType, pData, 10*1024);
			}
			else
			{
				Date_Len = HD_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, T_BuffsAnthor, nType, pData, 10*1024);
				//Date_Len = XL_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, T_BuffsAnthor, nType, pData, 10*1024);
			}
			
			if (Date_Len > 0)
			{
				bFlagDecode = 1;
				ioctl(BeepFd, JZ_LED_OFF, 0);
				iFlagLedStatus = 0;
			}
			
		}
		else
		{
			bFlagDecode = 0;
			
			if (iFlagLedStatus)
			{
				ioctl(BeepFd, JZ_LED_DELAY_OFF, 0);
				iFlagLedStatus = 0;
			}
			
			continue;
		}

		if ( Date_Len > 0)
		{
			//ioctl(BeepFd, DECODESUCCESS_SOUND, 0);

			iVoiceCtrl = 0x0;
					
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
			//printf("if (eBellTransmitStatus == BELLTRANSMITION_OPEN)...\n");
			
			if (eBellTransmitStatus == BELLTRANSMITION_OPEN)
			{					
				ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
			}

			if ((*pData == '$') && (strcmp(pData, "$02400") != 0))
			{
				SetModeFlag = 1;				
			}

			if ((SetModeFlag == 1) && (*pData == '$'))
			{
				EnterSetMode(pData);
			}

			if (SetModeFlag == 0)
			{
				ret = Decode_DataProcessing(pData, Date_Len);
				XL_RETURN(ret < 0, -1, "Decode_DataInversion error!");
			}

			if (strcmp(pData, "$02401") == 0)
			{
				SetModeFlag = 0;
			}

			//usleep(300000);
		}

		gettimeofday(&Tval_End, NULL);
		XL_TRACE_DEBUG("nead time:%ldus......\n", (Tval_End.tv_sec-Tval_Start.tv_sec)*1000000+(Tval_End.tv_usec-Tval_Start.tv_usec));
	}
	
	stop_Capture();
	
	close(BeepFd);	

	return (0);
}






