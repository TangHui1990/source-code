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
#include <sched.h>
#include <stdlib.h>
#include <malloc.h>
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


int XL_MT9V022_QrCodeDealWith(void)
{
	int Date_Len;
	struct timeval Tval_Start;
	struct timeval Tval_End;

	struct timeval Tval_Start1;
	struct timeval Tval_End1;
	int ret = -3;
	int buf_index;
	unsigned int ButtonVal = 1;
	int flg=0;
	char report[8];
	int flags;
	int ledFlag = 0;

	char *img = NULL;
#if 1
	/****************************test**********************/
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
#if 1
	errorInfo = dlerror();										
     	if (errorInfo != NULL){
         	printf("Dlsym Error:%s.\n",errorInfo);
         	return -1;
    	 }

		printf("Now ZDecode is avaliable\n");
#endif
#endif

	/*****************************end**************************/
	
	char *pData = NULL;
	int *nType = (int*)malloc(sizeof(int));
	XL_ASSERT(nType == NULL, "malloc nType error!!\n");

	pData = (char*)malloc(10*1024);
	XL_ASSERT(pData == NULL, "malloc pData error!!\n");

	char * T_Buffs = (char*)malloc(buffers[0].length);
	XL_ASSERT(T_Buffs == NULL, "malloc error!\n");

	char *T_BuffsAnthor = (char *)malloc(buffers[0].length);
	XL_ASSERT(T_BuffsAnthor == NULL, "malloc error!\n");

	while ( 1 ) 
	{	
		//gettimeofday(&Tval_Start1, NULL);
		SetModeFlag = 0;
		Date_Len = 0;

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
		
		gettimeofday(&Tval_Start, NULL);
		memset(pData, '\0', 10*1024);

		read(buttons_fd, &ButtonVal, 4);
		if (((ButtonVal>>31) & 0x1) == 1)
		{	
			ioctl(BeepFd, JZ_LED_OFF, 0);

			ledFlag = 0;
		}
		else if ((ledFlag == 0) && (((ButtonVal>>31) & 0x1) == 0))
		{
			ioctl(BeepFd, JZ_LED_ON, 0);
		}

		buf_index = readFrame();	


		if(buf_index == -1)
		{			
			continue;
		}
		
		memset(pData, '\0', 10*1024);

		if (((iSetCtrlRightStatus == CONTROL_UNLIMITED) || (iSetCtrlRightStatus == BARCODE_SET_PROHIBITED))&&(iComTriggerSwitchStatus ==COM_READ_WRITE_ALLOWED))
		{
			if ((ret = uart_read(pData, 10*1024)) > 0)
			{
				XL_TRACE_DEBUG("Read:%s, ret = %d\n", pData, ret);

				if ( XL_IsSetCodeInput(pData, ret) == 1)					/* 判断是否为设置码 */
				{
					SetModeFlag = 1;
				}
			}

			if (SetModeFlag == 1)
			{
				if ( eCommandEchoStatus == SERIAL_COMMAND_RETURN )			/* 串口命令回应 */
				{
					int iCmdRsp = 54;
					uart_write(&iCmdRsp, sizeof(iCmdRsp));
				}
				
				EnterSetMode(pData+6);
			}
		}

		//OutPutYuv(0, buffers[buf_index].start, PREVIEW_WIDTH, PREVIEW_HEIGHT);

		memset(pData, '\0', 10*1024);

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

		memcpy(yuv, buffers[buf_index].start, PREVIEW_WIDTH*PREVIEW_HEIGHT);

		int i,j;
		for (j=0; j<PREVIEW_HEIGHT; j++)
			for(i=0; i<PREVIEW_WIDTH; i++)
				img[j*PREVIEW_WIDTH+i] = yuv[(PREVIEW_HEIGHT-1-j)*PREVIEW_WIDTH + i];

		if (((ButtonVal>>31) & 0x1) == 0 && eScanModeStatus == SCANMODE_MANUAL && ledFlag == 0)
		{
			//gettimeofday(&Tval_Start, NULL);
			//Date_Len = HD_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, buffers[buf_index].start, nType, pData, 10*1024);	
			//Date_Len = XL_iDecodeBar(PREVIEW_WIDTH, PREVIEW_HEIGHT, buffers[buf_index].start, nType, pData, 10*1024);
			//gettimeofday(&Tval_End, NULL);
			//XL_TRACE_DEBUG("decode time:%ldus......\n", (Tval_End.tv_sec-Tval_Start.tv_sec)*1000000+(Tval_End.tv_usec-Tval_Start.tv_usec));
			//ZDecode(buffers[buf_index].start,  PREVIEW_WIDTH, PREVIEW_HEIGHT, pData);
#if 1
			(*ZDecode_method)(img, PREVIEW_WIDTH, PREVIEW_HEIGHT, pData);

			if (pData[0] != '\0')
     		{
     			printf("result = %s\n",pData);
     		}
			else{
				//printf("ZDcode fail\n");
     		}

			while(pData[Date_Len]!='\0'){
			Date_Len++;
			
			}
			#endif
		}

		free(img);
		free(yuv);
		
		if ( Date_Len > 0 )
		{
			ledFlag = 1;
			ioctl(BeepFd, JZ_LED_OFF, 0);		/* 补光灯 */

			if (eLedTipsStatus == LEDTIPS_OPEN)
			{
				ioctl(BeepFd, JZ_LED_TIP_OFF, 0);  	/* LED提示灯 */
			}
			
			if(flg == 0)
			{	
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

				printf("if (eBellTransmitStatus == BELLTRANSMITION_OPEN)...\n");
				
				if (eBellTransmitStatus == BELLTRANSMITION_OPEN)
				{					
					ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
				}
			
				if ( (iSetCtrlRightStatus == CONTROL_UNLIMITED) || (iSetCtrlRightStatus == COM_SET_PROHIBITED) )
				{

					if ( XL_IsSetCodeInput(pData, Date_Len) == 1)
					{
						SetModeFlag = 1;

						EnterSetMode(pData+6);
					}
			#if 0
					if ((SetModeFlag == 1) && (strcmp(pData, "*B00*") == 0))
					{
						EnterSetMode(pData);
			
						XL_TRACE_DEBUG("Enter upgrade mode...\n\n");
			
						close(BeepFd);
						close(buttons_fd);
						closeCamera();
							
						XL_UpgradeForMassStorageRunning();
					}
					#endif
				}
						
				if (SetModeFlag == 0)
				{
					ret = Decode_DataProcessing(pData, Date_Len);
					XL_RETURN(ret < 0, -1, "Decode_DataInversion error!");
				}

				if (eRedSameCodeStatus==READSAMECODE_OPEN) /* 相同条码读取开启 */
				{
					flg = 0;
				}
				else		/* 相同条码读取关闭 */
				{
					flg = 1;
				}							
			}						
		}
		else 
		{
			flg = 0;
		}
			
		//gettimeofday(&Tval_End1, NULL);
		//XL_TRACE_DEBUG("total need time:%ldus......\n", (Tval_End1.tv_sec-Tval_Start1.tv_sec)*1000000+(Tval_End1.tv_usec-Tval_Start1.tv_usec));
	}
	
	stop_Capture();	
	close(BeepFd);	

	//dlclose(libzzm_handle);

	return (0);
}





