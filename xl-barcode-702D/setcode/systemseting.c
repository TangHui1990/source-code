#include "systemseting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>

#include <control.h>
#include <xl_debug.h>

unsigned int SETTAB[256];

NakHandshake NAKStatus = HANDSHAKE_CLOSE;  					/* NAK握手 *///解码数据输出，主机需要回复一个ACK，如果回复NAK，数据重发，如果超时，就相当收到ACK，不在重发 //ok
HardwareContolFlow iHdflowStatus = HARDWARECONTROLFLOW_CLOSE;			/* 硬件控制流 */	//虚拟串口无硬件控制流一说		
XonHandshake iXONStatus = XONHANDSHAKE_CLOSE;		//ok			/* XON握手 *///串口收到0x13，停止发送数据，0X11开始发送数据，pos机那边每多少ms就会给扫描器发送这些数据
ComTriggerSwitch iComTriggerSwitchStatus = COM_READ_WRITE_ALLOWED;	/* COM触发开关 */ //ok
SetContolRight iSetCtrlRightStatus = CONTROL_UNLIMITED;  		/* 设置控制权 */ //ok
SerialportSetVoice eSerialPortStatus = SERIALPORTSETVOICE_CLOSE; 		/* 串口设置提示音 */ //ok
FunctionalKeyboard eFuncKeyStatus;			/* 功能键盘 */
Keypad eFunckeypadStatus;					/* 小键盘 */
CaseIgnored eCaseIgnoreStatus = CASEIGNORED_OPEN;				/* 大小写忽略 */ //ok
AIMcode eAimCodeSetStatus;					/* AIM码 *///跟徐工要文档 //以后再做
CodeID eCodeIdSetStatus = CODEID_CLOSE;		//ok			/* CODE ID *///解码的时候，会加一个码制相关的标志，然后看文档，查询她是什么码制，然后在HID前面加前缀
Febraban eFebrabanFuncSetStatus;			/* Febraban *///解码相关不管
BootSound eBootSoundSetStatus = BOOTSOUND_OPEN;				/* 开机音 */	 //ok
DataInversion eDecodeDataInversionStatus = DATAINVERSION_CLOSE;	/* 数据倒置 */  //0k
LengthPrefix eLengthPrefixSetStatus = LENGTHPREFIX_CLOSE;		/* 长度前缀 */ //ok
BellTransmition eBellTransmitStatus = BELLTRANSMITION_OPEN;		/* 响铃传输 *///解码成功，先响蜂鸣器还是先传输，顺序先随便
BELfunc eBleFuncSetStatus = BELFUNC_CLOSE;			//ok		/* BEL功能 *///电脑发0x07给我，我接收到以后响蜂鸣器，开启的话蜂鸣器不响
SleepTipVioce eSleepTipVioceStatus = SLEEPTIPVIOCE_CLOSE;			/* 休眠提示音 */ //声音待具体调试
Tropitic eTropiticStatus;					/* tropitic39 *///解码相关不管
AutomaticInduction eAutomaticInductionStatus;/* 自动感应 *///解码相关不管
EANstrengthen eEanStrenthenSetStatus;		/* EAN13加强 *///解码相关不管
ISBTstatus eIsbtStatusSetStatus;			/* ISBT *///解码相关不管
LedTips eLedTipsStatus = LEDTIPS_OPEN;		//ok				/* LED提示 *///读取成功，led闪，上电是常亮，读取成功灭50ms（用定时器），然后再亮
ReadSameCode eRedSameCodeStatus = READSAMECODE_CLOSE;			/* 相同条码读取 */ //0k
ReadPresupposition eReadPresuppositionStatus;/* 读取预设 *///先不弄
FillinLight eFillinLightSetStatus;			/* 补光灯 *///现在实现关闭，开启
MarkLamp eMarkLampStatus;					/* 标示灯 *///开启，关闭
CommandEcho eCommandEchoStatus = SERIAL_COMMAND_NO_RETURN;		//ok		/* 命令回应 *///succeed ACK:dec 06    fail:NAK dec 21
Interface eInterfaceStatus = INTERFACE_VCP;					/* 接口 */ //ok
Language eLanguageStatus = LANGUAGE_USA;					/* 语言 */  //没有测试
BaudRate eBaudRateStatus = BAUDRATE_115200N;					/* 波特率 */ //ok
DataBit eDataBitStatus = DATABIT_7BIT;						/* 数据位 */ //ok
CheckoutBit eCheckoutBitStatus = CHECKOUTBIT_NONE;				/* 校验位 */ //ok
StopBit eStopBitStatus = STOPBIT_1BIT;						/* 停止位 */
Terminator eTerminatorStatus = TERMINATOR_TAB;				/* 结束符 *///输出后面加 //ok
CaseAndCase eCaseAndCaseStatus = CASEANDCASE_RAWDATA;	/* 大小写 */ //0k
ScanMode eScanModeStatus = SCANMODE_MANUAL;					/* 扫描方式 *///只实现了手动模式
RedundAdjust eRedundAdjustStatus;			/* 冗余裁决 */
SoundLength eSoundLengthStatus = SOUNDLENGTH_SHORT;				/* 音长 *///50ms,200ms,500ms  //ok
AudioFrequency eAudioFrequencyStatus = AUDIOFREQUENCY_HIGH;		/* 音频 *///低频，800Hz,1200, 1500 //ok 
Volume eVolumeStatus;						/* 音量 */	//后面再做
KBclock eKBclockStatus;						/* KB时钟 */ //先不做
DataInterception eDataInterceptionStatus;	/* 数据截取 */ //增加数据开始位，结束位，然后实现
OneReverseColor eOneReverseColorStatus;		/* 一维反色 *///解码相关，不用实现
TwoReverseColor eTwoReverseColorStatus;		/* 二维反色 *///解码相关，不用实现
OneReadMore eOneReadMoreStatus;				/* 一维多读 *///解码相关，不用实现
TwoReadMore eTwoReadMoreStatus;				/* 二维多读 *///解码相关，不用实现
DecodeSleep eDecodeSleepStatus;				/* 解码休眠 *///低功耗相关，现在做不了
EngineSleep eEngineSleepStatus;				/* 引擎休眠 *///低功耗相关，现在做不了
ReadDelay eReadDelayStatus;					/* 读取延时 *///先不做
AdditionCodeRdDelay eAdditionCodeRdDelayStatus;/* 附加码读取延时 *///做不了
AuxStatus eAuxStatusStatus;					/* AUX *///做不了


OutputMode eOutModeStatus = HID_OUTPUT;
CapsLockMode eCapsLockStatus = CAPSLOCK_NOPRESS;

extern int UartFd;
extern int BeepFd;
extern int iVoiceCtrl;

unsigned char cFlagXon = 1;

/**********************************************************************
 *  Function Name：Hardware_Controlflow
 *	Function description：硬件控制流
 *   
 *	Function parameter:      iHdflowchoice: HARDWARECONTROLFLOW_CLOSE ---> 控制流关闭
 											HARDWARECONTROLFLOW_OPEN  ---> 控制流开启
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int Hardware_Controlflow(HardwareContolFlow iHdflowStatus)
{
	if (iHdflowStatus == HARDWARECONTROLFLOW_CLOSE)			/* 控制流关闭 */
	{
	}
	else if(iHdflowStatus == HARDWARECONTROLFLOW_OPEN)		/* 控制流开启 */
	{
	}
	
	return (0);
}

/**********************************************************************
 *  Function Name：XON_Handshake
 *	Function description：XON握手
 *   
 *	Function parameter:      iXONchoice: XONHANDSHAKE_OPEN  ---> XON握手开启
 										 XONHANDSHAKE_CLOSE ---> XON握手关闭
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int XON_Handshake(XonHandshake iXONStatus)
{
	if (iXONStatus == XONHANDSHAKE_OPEN)				/* XON握手开启 */
	{
	}
	else if(iXONStatus == XONHANDSHAKE_CLOSE)			/* XON握手关闭 */
	{
	}
	
	return (0);
}

/**********************************************************************
 *  Function Name：Com_TriggerSwitch
 *	Function description：COM触发开关
 *   
 *	Function parameter:      iComTriggerSwitchStatus: COM_READ_WRITE_ALLOWED ---> COM充许触发读写
 										 			  TRIGGER_UNLIMITED ---> 不充许
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int Com_TriggerSwitch(ComTriggerSwitch iComTriggerSwitchStatus)
{
	if (iComTriggerSwitchStatus == COM_READ_WRITE_ALLOWED)	/* COM充许触发读写 */
	{
	}
	else if(iComTriggerSwitchStatus == TRIGGER_UNLIMITED)	/* 不充许 */
	{
	}

	return (0);
}

/**********************************************************************
 *  Function Name：Com_TriggerSwitch
 *	Function description：设置控制权
 *   
 *	Function parameter:      iSetCtrlRightStatus: COM_READ_WRITE_ALLOWED ---> COM充许触发读写
 										 		  TRIGGER_UNLIMITED ---> 不充许
 										 		  COM_SET_PROHIBITED ---> 禁止COM设置
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int Set_ControlRight(SetContolRight iSetCtrlRightStatus)
{
	if (iSetCtrlRightStatus == CONTROL_UNLIMITED)			/* 无限制 */
	{
	}
	else if(iSetCtrlRightStatus == BARCODE_SET_PROHIBITED)	/* 禁止条码设置 */
	{
	}
	else if(iSetCtrlRightStatus == COM_SET_PROHIBITED)		/* 禁止COM设置 */
	{
	}

	return (0);
}


/**********************************************************************
 *  Function Name：Function_Keyboard_Set
 *	Function description：功能键盘
 *   
 *	Function parameter: eFuncKeyStatus: FUNCTIONALKEYBOARD_CLOSE---> 功能键盘关闭
 										FUNCTIONALKEYBOARD_OPNE ---> 功能键盘开启
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int Function_Keyboard_Set(FunctionalKeyboard eFuncKeyStatus)
{
	if (eFuncKeyStatus == FUNCTIONALKEYBOARD_CLOSE)			/* 功能键盘关闭 */
	{
	}
	else if(eFuncKeyStatus == FUNCTIONALKEYBOARD_OPNE)		/* 功能键盘开启 */
	{
	}

	return (0);
}


/**********************************************************************
 *  Function Name：Function_Keypad_Set
 *	Function description：小键盘
 *   
 *	Function parameter: eFunckeypadStatus: KEYPAD_CLOSE---> 小键盘关闭
 										   KEYPAD_OPEN ---> 小键盘开启
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
 int Function_Keypad_Set(Keypad eFunckeypadStatus)
{
	if (eFunckeypadStatus == KEYPAD_CLOSE)			/* 小键盘关闭 */
	{
	}
	else if(eFunckeypadStatus == KEYPAD_OPEN)		/* 小键盘开启 */
	{
	}

	return (0);
}


 /**********************************************************************
 *  Function Name：AIM_Code_Set
 *	Function description：AIM码
 *   
 *	Function parameter: eAimCodeSetStatus: AIMCODE_CLOSE---> AIM码关闭
 										   AIMCODE_OPEN ---> AIM码开启
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
 int AIM_Code_Set(AIMcode eAimCodeSetStatus)
 {
 	if (eAimCodeSetStatus == AIMCODE_CLOSE)			/* AIM码关闭 */
 	{
 	}
	else if(eAimCodeSetStatus == AIMCODE_OPEN)		/* AIM码开启 */
	{
	}
	
 	return (0);
 }


 /**********************************************************************
 *  Function Name：Code_ID_Set
 *	Function description：CODE ID
 *   
 *	Function parameter: eCodeIdSetStatus: CODEID_CLOSE---> CODE ID 关闭
 										   CODEID_OPEN ---> CODE ID 开启
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int Code_ID_Set(CodeID eCodeIdSetStatus)
{
	if (eCodeIdSetStatus == CODEID_CLOSE)			/* CODE ID 关闭 */
	{
	}
	else if(eCodeIdSetStatus == CODEID_OPEN)		/* CODE ID 开启 */
	{
	}
	
	return (0);
}

/**********************************************************************
*  Function Name：Febraban_Func_Set
*  Function description：Febraban
*	
*  Function parameter: eFebrabanFuncSetStatus: FEBRABAN_CLOSE---> Febraban 关闭
										  	   FEBRABAN_OPEN ---> Febraban 开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Febraban_Func_Set(Febraban eFebrabanFuncSetStatus)
{
	if (eFebrabanFuncSetStatus == FEBRABAN_CLOSE)		/* Febraban 关闭 */
	{
	}
	else if(eFebrabanFuncSetStatus == FEBRABAN_OPEN)	/* Febraban 开启 */
	{
	}

	return (0);
}

/**********************************************************************
 *  Function Name：Decode_DataInversion
 *	Function description：HID输出数据处理
 *   
 *	Function parameter:      source input: pSrcDat 
 *							 data lenth: iLen
 *	return: success return 0
 * 			error return  -1
 **********************************************************************/
int iHandshakeFlag = 1;

int Decode_DataProcessing(char *pSrcDat, int iLen)
{
	int index;
	char pDstDat[iLen+1];
	char pTmpDat[iLen];
	char pLenStr[6+iLen];

	unsigned char iack;
	int nakfd;
	int nakvalue = 1;
	int ret;

	int CodeIdNum = 0;   //test;
	char CodeIdStr[6+iLen+2];
	char CodeId[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
		'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	
	XL_RETURN(pSrcDat == NULL , -1, "pSrcDat is NULL!\n");

	if (eCaseAndCaseStatus == CASEANDCASE_RAWDATA)			/* 原数据 */
	{
		memcpy(pTmpDat, pSrcDat, iLen);

		pTmpDat[iLen] = '\0';
	}
	else if(eCaseAndCaseStatus == CASEANDCASE_UPPERCASE)	/* 强制大写 */
	{	
		for (index = 0; index < iLen; index++)
		{
			if (isalpha(pSrcDat[index]) && islower(pSrcDat[index]))
			{
				pSrcDat[index] = toupper(pSrcDat[index]);
			}
		}
	
		memcpy(pTmpDat, pSrcDat, iLen);
	
		pTmpDat[iLen] = '\0';
	}
	else if(eCaseAndCaseStatus == CASEANDCASE_LOWERCASE)	/* 强制小写 */
	{	
		for (index = 0; index < iLen; index++)
		{
			if (isalpha(pSrcDat[index]) && isupper(pSrcDat[index]))
			{
				pSrcDat[index] = tolower(pSrcDat[index]);
			}
		}
	
		memcpy(pTmpDat, pSrcDat, iLen);
	
		pTmpDat[iLen] = '\0';
	}
	else if(eCaseAndCaseStatus == CASEANDCASE_UPPERLOWEREXCHANGE)	/* 大小写互换 */
	{	
		for (index = 0; index < iLen; index++)
		{
			if (isalpha(pSrcDat[index]) && isupper(pSrcDat[index]))
			{
				pSrcDat[index] = tolower(pSrcDat[index]);
			}
			else if (isalpha(pSrcDat[index]) && islower(pSrcDat[index]))
			{
				pSrcDat[index] = toupper(pSrcDat[index]);
			}
		}
	
		memcpy(pTmpDat, pSrcDat, iLen);
	
		pTmpDat[iLen] = '\0';
	}

	if (DATAINVERSION_OPEN == eDecodeDataInversionStatus)				/* 数据倒置开启 */
	{
		for (index = 0; index < iLen; index++)
		{
			pDstDat[index] = pTmpDat[iLen-index-1];	
		}

		pDstDat[iLen] = '\0';
	}
	else if(DATAINVERSION_CLOSE == eDecodeDataInversionStatus)			/* 数据倒置关闭 */
	{
		memcpy(pDstDat, pTmpDat, iLen);

		pDstDat[iLen] = '\0';
	}

	if (LENGTHPROFIX_OPEN == eLengthPrefixSetStatus) 					/* 长度前缀开启 */
	{
		sprintf(pLenStr, "%d", iLen);

		strcat(pLenStr, pDstDat);
	}
	else if(LENGTHPREFIX_CLOSE == eLengthPrefixSetStatus)				/* 长度前缀关闭 */
	{
		memset(pLenStr, '\0', sizeof(pLenStr));
		
		memcpy(pLenStr, pDstDat, iLen);

		pDstDat[iLen] = '\0';
	}

	if (eCodeIdSetStatus == CODEID_OPEN)								/* code ID 开启 */
	{
		CodeIdStr[0] = CodeId[CodeIdNum];
		CodeIdStr[1] = '\0';

		strcat(CodeIdStr, pLenStr);
	}
	else if(eCodeIdSetStatus == CODEID_CLOSE)							/* code ID 关闭 */
	{
		memset(CodeIdStr, '\0', sizeof(CodeIdStr));
		
		memcpy(CodeIdStr, pLenStr, sizeof(pLenStr));
	}

	/* 输出 */
	//if (eInterfaceStatus == INTERFACE_HID)
	//{
		Hid_Output(CodeIdStr, strlen(CodeIdStr));

		if (eBellTransmitStatus == BELLTRANSMITION_CLOSE)
		{
			unsigned char cBell;
			if (eBleFuncSetStatus == BELFUNC_OPEN)
			{
				if (uart_read(&cBell, 1) > 0)
				{
					tcflush(UartFd, TCIOFLUSH);
				
					if (cBell == 0x07)
					{
						ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
					}
				}
			}
			else if(eBleFuncSetStatus == BELFUNC_CLOSE)
			{
				ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
			}
		}
	//}
	//else if((eInterfaceStatus == INTERFACE_VCP) && (iComTriggerSwitchStatus == COM_READ_WRITE_ALLOWED))
	//{
		unsigned char cXON;
		
		if (iXONStatus == XONHANDSHAKE_OPEN)								/* XON握手开启 */
		{
			if (uart_read(&cXON, 1) > 0)
			{	
				tcflush(UartFd, TCIOFLUSH);
				
				if (cXON == 0x11)
				{
					cFlagXon = 1;
				}
				else if(cXON == 0x13)
				{
					cFlagXon = 0;
				}
			}
		}
		else if(iXONStatus == XONHANDSHAKE_CLOSE)
		{
			cFlagXon = 1;
		}

		if ( cFlagXon )
		{
			
			uart_write(CodeIdStr, strlen(CodeIdStr));

			if (eBellTransmitStatus == BELLTRANSMITION_CLOSE)
			{	
				unsigned char cBell;
				if (eBleFuncSetStatus == BELFUNC_OPEN)
				{
					if (uart_read(&cBell, 1) > 0)
					{	
						tcflush(UartFd, TCIOFLUSH);
					
						if (cBell == 0x07)
						{
							ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
						}
					}
				}
				else if(eBleFuncSetStatus == BELFUNC_CLOSE)
				{
					ioctl(BeepFd, DECODESUCCESS_SOUND, &iVoiceCtrl);
				}
				
			}

			if (NAKStatus == HANDSHAKE_OPEN)
			{
				iHandshakeFlag = 1;
				nakvalue = 0;

				nakfd = open("/dev/mynak", O_RDWR);
				XL_RETURN(nakfd<0, -1, "Open /dev/mynak error!");

				ret = ioctl(nakfd, NAK_TIMER_50MS, 0);
				XL_RETURN(ret<0, -1, "ioctl error!\n");

				tcflush(UartFd, TCIOFLUSH);
				
				while (iHandshakeFlag)
				{
					if (uart_read(&iack, 1) > 0)
					{
						if (iack == 21)
						{
							uart_write(CodeIdStr, strlen(CodeIdStr));
						}
						else if(iack == 6)
						{
							break;
						}
					}

					read(nakfd, &nakvalue, 4);
					if (nakvalue == 1)
					{
						iHandshakeFlag = 0;
					}
					printf("+++read:%d\n", nakvalue);
				}

				close(nakfd);
			}
		}		
	//}
	
	return (0);
}

/**********************************************************************
*  Function Name：Length_Prefix_Set
*  Function description：长度前缀
*	
*  Function parameter: eLengthPrefixSetStatus:  LENGTHPREFIX_CLOSE---> 长度前缀关闭
										  		LENGTHPROFIX_OPEN ---> 长度前缀开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Length_Prefix_Set(LengthPrefix eLengthPrefixSetStatus)
{
	if (eLengthPrefixSetStatus == LENGTHPREFIX_CLOSE)		/* 长度前缀关闭 */
	{
	}
	else if(eLengthPrefixSetStatus == LENGTHPROFIX_OPEN)	/* 长度前缀开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Bell_Transmition_Set
*  Function description：响铃传输
*	
*  Function parameter: eBellTransmitStatus:  BELLTRANSMITION_CLOSE---> 响铃传输关闭
										  	 BELLTRANSMITION_OPEN ---> 响铃传输开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Bell_Transmition_Set(BellTransmition eBellTransmitStatus)
{
	if (eBellTransmitStatus == BELLTRANSMITION_CLOSE)		/* 响铃传输关闭 */
	{
	}
	else if(eBellTransmitStatus == BELLTRANSMITION_OPEN)	/* 响铃传输开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：BEL_Func_Set
*  Function description：BEL功能
*	
*  Function parameter: eBleFuncSetStatus:  BELFUNC_CLOSE---> BEL功能关闭
										   BELFUNC_OPEN ---> BEL功能开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int BEL_Func_Set(BELfunc eBleFuncSetStatus)
{
	if (eBleFuncSetStatus == BELFUNC_CLOSE)			/* BEL功能关闭 */
	{
	}
	else if(eBleFuncSetStatus == BELFUNC_OPEN)		/* BEL功能开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Sleep_Tip_Vioce
*  Function description：休眠提示音
*	
*  Function parameter: eSleepTipVioceStatus:  SLEEPTIPVIOCE_CLOSE---> 休眠提示音关闭
										   SLEEPTIPVIOCE_OPEN ---> 休眠提示音开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Sleep_Tip_Vioce(SleepTipVioce eSleepTipVioceStatus)
{
	if (eSleepTipVioceStatus == SLEEPTIPVIOCE_CLOSE)		/* 休眠提示音关闭 */
	{
	}
	else if(eSleepTipVioceStatus==SLEEPTIPVIOCE_OPEN)		/* 休眠提示音开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Tropitic_Func_Set
*  Function description：tropitic39
*	
*  Function parameter: eTropiticStatus:  TROPITIC_CLOSE---> tropitic39关闭
										 TROPITIC_OPEN ---> tropitic39开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Tropitic_Func_Set(Tropitic eTropiticStatus)
{
	if (eTropiticStatus == TROPITIC_CLOSE)			/* tropitic39关闭 */
	{
	}
	else if(eTropiticStatus == TROPITIC_OPEN)		/* tropitic39开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Automatic_Induction_Set
*  Function description：自动感应
*	
*  Function parameter: eAutomaticInductionStatus:   AUTOMATICINDUCTION_CLOSE---> 自动感应关闭
										 			AUTOMATICINDUCTION_OPEN ---> 自动感应开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Automatic_Induction_Set(AutomaticInduction eAutomaticInductionStatus)
{
	if (eAutomaticInductionStatus == AUTOMATICINDUCTION_CLOSE)		/* 自动感应关闭 */
	{
	}
	else if(eAutomaticInductionStatus == AUTOMATICINDUCTION_OPEN)	/* 自动感应开启 */
	{
	}
	
	return (0);
}

/**********************************************************************
*  Function Name：EAN_Strengthen_Set
*  Function description：EAN13加强
*	
*  Function parameter: eEanStrenthenSetStatus:  EANSTRENGTHEN_CLOSE---> EAN13加强关闭
										 		EANSTRENGTHEN_OPEN ---> EAN13加强开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int EAN_Strengthen_Set(EANstrengthen eEanStrenthenSetStatus)
{
	if (eEanStrenthenSetStatus == EANSTRENGTHEN_CLOSE)		/* EAN13加强关闭 */
	{
	}
	else if(eEanStrenthenSetStatus == EANSTRENGTHEN_OPEN)	/* EAN13加强开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：ISBT_Status_Set
*  Function description：ISBT
*	
*  Function parameter: eIsbtStatusSetStatus:  ISBT_CLOSE---> ISBT关闭
										 	  ISBT_OPEN ---> ISBT开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int ISBT_Status_Set(ISBTstatus eIsbtStatusSetStatus)
{
	if (eIsbtStatusSetStatus == ISBT_CLOSE)			/* ISBT关闭 */
	{
	}
	else if(eIsbtStatusSetStatus == ISBT_OPEN)		/* ISBT开启 */
	{
	}

	return (0);
}


/**********************************************************************
*  Function Name：Led_Tips_Set
*  Function description：LED提示
*	
*  Function parameter: eLedTipsStatus:  LEDTIPS_CLOSE---> LED提示关闭
										LEDTIPS_OPEN ---> LED提示开启
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Led_Tips_Set(LedTips eLedTipsStatus)
{
	if (eLedTipsStatus == LEDTIPS_CLOSE)		/* LED提示关闭 */
	{
	}
	else if(eLedTipsStatus == LEDTIPS_OPEN)		/* LED提示开启 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Read_Presupposition_Set
*  Function description：读取预设
*	
*  Function parameter: eReadPresuppositionStatus:   READpresupposition_AUTO---> 读取预设自动
*													READpresupposition_MONITOR---> 读取预设显示器
*													READpresupposition_PAPER--->读取预设纸张
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Read_Presupposition_Set(ReadPresupposition eReadPresuppositionStatus)
{
	if (eReadPresuppositionStatus == READpresupposition_AUTO)			/* 读取预设自动 */
	{
	}
	else if(eReadPresuppositionStatus == READpresupposition_MONITOR)	/* 读取预设显示器 */
	{
	}
	else if(eReadPresuppositionStatus == READpresupposition_PAPER)		/* 读取预设纸张 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Fillin_Light_Set
*  Function description：补光灯
*	
*  Function parameter: eFillinLightSetStatus:   FILLINLIGHT_CLOSE---> 补光灯关闭
*												FILLINLIGHT_LOWBRIGHTNESS---> 补光灯低亮度
*												FILLINLIGHT_MEDIUMBRIGHTNESS--->补光灯中亮度
*												FILLINLIGHT_HIGHBRIGHTNESS--->补光灯高亮度
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Fillin_Light_Set(FillinLight eFillinLightSetStatus)
{
	if (eFillinLightSetStatus == FILLINLIGHT_CLOSE)					/* 补光灯关闭 */
	{
	}
	else if(eFillinLightSetStatus == FILLINLIGHT_LOWBRIGHTNESS)		/* 补光灯低亮度 */
	{
	}
	else if(eFillinLightSetStatus == FILLINLIGHT_MEDIUMBRIGHTNESS)	/* 补光灯中亮度 */
	{
	}
	else if(eFillinLightSetStatus == FILLINLIGHT_HIGHBRIGHTNESS)	/* 补光灯高亮度 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Mark_Lamp_Set
*  Function description：标示灯
*	
*  Function parameter: eMarkLampStatus:   MARKLAMP_CLOSE---> 补光灯关闭
*										  MARKLAMP_OPEN---> 补光灯低亮度
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Mark_Lamp_Set(MarkLamp eMarkLampStatus)
{
	if (eMarkLampStatus == MARKLAMP_CLOSE)		/* 标示灯关 */
	{
	}
	else if(eMarkLampStatus == MARKLAMP_OPEN)	/* 标示灯开 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Command_Echo_Set
*  Function description：命令回应
*	
*  Function parameter: eCommandEchoStatus:   SERIAL_COMMAND_NO_RETURN---> 串口命令无返回
*										 	 SERIAL_COMMAND_RETURN---> 串口命令有返回
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Command_Echo_Set(CommandEcho eCommandEchoStatus)
{
	if (eCommandEchoStatus == SERIAL_COMMAND_NO_RETURN)		/* 串口命令无返回 */
	{
	}
	else if(eCommandEchoStatus == SERIAL_COMMAND_RETURN)	/* 串口命令有返回 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Interface_Func_Set
*  Function description：接口
*	
*  Function parameter: eInterfaceStatus:   INTERFACE_AUTO---> 自动
*										   INTERFACE_KB---> KB
*										   INTERFACE_HID-->HID
*										   INTERFACE_VCP-->VCP
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Interface_Func_Set(Interface eInterfaceStatus)
{
	if (eInterfaceStatus == INTERFACE_AUTO)					/* 自动 */
	{
	}
	else if(eInterfaceStatus == INTERFACE_KB)				/* KB */
	{
	}
	else if(eInterfaceStatus == INTERFACE_HID)				/* HID */
	{
	}
	else if(eInterfaceStatus == INTERFACE_VCP)				/* VCP */
	{
	}

	return (0);
}


/**********************************************************************
*  Function Name：Language_Func_Set
*  Function description：语言
*	
*  Function parameter: eLanguageStatus:   LANGUAGE_COMMON---> 通用
*										  LANGUAGE_USA---> 美国
*										  LANGUAGE_FRANCE-->法国
*										  LANGUAGE_GERMANY-->德国
*										  LANGUAGE_BRITAIN-->英国
*										  LANGUAGE_TURKEY-->土耳其
*										  LANGUAGE_DENMARK-->丹麦
*										  LANGUAGE_JAPAN-->日本
*										  LANGUAGE_SPAIN-->西班牙
*										  LANGUAGE_ITALY-->意大利
*										  LANGUAGE_JACK-->杰克
*										  LANGUAGE_SNOVUCK-->斯诺伐克
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Language_Func_Set(Language eLanguageStatus)
{
	if (eLanguageStatus == LANGUAGE_COMMON)			/* 通用 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_USA)		/* 美国 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_FRANCE)		/* 法国 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_GERMANY)	/* 德国 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_BRITAIN)	/* 英国 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_TURKEY)		/* 土耳其 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_DENMARK)	/* 丹麦 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_JAPAN)		/* 日本 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_SPAIN)		/* 西班牙 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_ITALY)		/* 意大利 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_JACK)		/* 杰克 */
	{
	}
	else if(eLanguageStatus == LANGUAGE_SNOVUCK)	/* 斯诺伐克 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：BaudRate_Func_set
*  Function description：波特率
*  Function parameter: eBaudRateStatus:   BAUDRATE_1200N---> 波特率1200
*										  BAUDRATE_2400N---> 波特率2400
*										  BAUDRATE_4800N-->	 波特率4800
*										  BAUDRATE_9600N-->	 波特率9600
*										  BAUDRATE_14400N--> 波特率14400
*										  BAUDRATE_19200N--> 波特率19200
*										  BAUDRATE_28800N--> 波特率28800
*										  BAUDRATE_38400N--> 波特率38400
*										  BAUDRATE_57600N--> 波特率57600
*										  BAUDRATE_115200N-->波特率115200
*  return: success return 0
*		   error return  -1
**********************************************************************/
int BaudRate_Func_set(void)
{
	int ret;
	
	if (eBaudRateStatus == BAUDRATE_1200N)			/* 波特率1200 */
	{
		ret = set_baudrate(UartFd, 1200);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B1200 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_2400N)		/* 波特率2400 */
	{
		ret = set_baudrate(UartFd, 2400);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B2400 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_4800N)		/* 波特率4800 */
	{
		ret = set_baudrate(UartFd, 4800);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B4800 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_9600N)		/* 波特率9600 */
	{
		ret = set_baudrate(UartFd, 9600);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B9600 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_14400N)		/* 波特率14400 */  //非标准波特率不予支持	
	{
		//ret = set_baudrate(UartFd, 14400);
		//XL_RETURN(ret < 0, -1, "Set BaudRate is B14400 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_19200N)		/* 波特率19200 */
	{
		ret = set_baudrate(UartFd, 19200);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B19200 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_28800N)		/* 波特率28800 */ //非标准波特率不予支持	
	{
		//ret = set_baudrate(UartFd, 28800);
		//XL_RETURN(ret < 0, -1, "Set BaudRate is B28800 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_38400N)		/* 波特率38400 */
	{
		ret = set_baudrate(UartFd, 38400);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B38400 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_57600N)		/* 波特率57600 */
	{
		ret = set_baudrate(UartFd, 57600);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B57600 error!\n");
	}
	else if(eBaudRateStatus == BAUDRATE_115200N)	/* 波特率115200 */
	{
		ret = set_baudrate(UartFd, 115200);
		XL_RETURN(ret < 0, -1, "Set BaudRate is B115200 error!\n");
	}

	return (0);
}

/**********************************************************************
*  Function Name：Data_Bit_Set
*  Function description：数据位
*	
*  Function parameter: eDataBitStatus:   DATABIT_7BIT---> 7位
*										 DATABIT_8BIT---> 8位
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Data_Bit_Set(void)
{
	int ret;
	
	if (eDataBitStatus == DATABIT_7BIT)			/* 7位 */
	{
		ret = set_databit(UartFd, 7);
		
		XL_RETURN(ret < 0, -1, "Set Serial Data is 7 Bit error!");
	}
	else if(eDataBitStatus == DATABIT_8BIT)		/* 8位 */
	{
		ret = set_databit(UartFd, 8);
		
		XL_RETURN(ret < 0, -1, "Set Serial Data is 8 Bit error!");
	}

	return (0);
}


/**********************************************************************
*  Function Name：Checkout_Bit_Set
*  Function description：校验位
*  Function parameter: eCheckoutBitStatus:   CHECKOUTBIT_NONE---> 无校验
*										  CHECKOUTBIT_ODD--->  奇校验
*										  CHECKOUTBIT_PARITY-->偶校验
*										  CHECKOUTBIT_EMPTY--> 空校验
*										  CHECKOUTBIT_LABLE--> 标记校验
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Checkout_Bit_Set(void)
{
	int ret; 
	
	if (eCheckoutBitStatus == CHECKOUTBIT_NONE)			/* 无校验 */
	{
		ret = set_Paritycheck(UartFd, 'N');

		XL_RETURN(ret < 0, -1, "Set Serial CheckOut is CHECKOUTBIT_NONE error!");
	}
	else if(eCheckoutBitStatus == CHECKOUTBIT_ODD)		/* 奇校验 */
	{
		ret = set_Paritycheck(UartFd, '0');

		XL_RETURN(ret < 0, -1, "Set Serial CheckOut is CHECKOUTBIT_ODD error!");
	}
	else if(eCheckoutBitStatus == CHECKOUTBIT_PARITY)	/* 偶校验 */
	{
		ret = set_Paritycheck(UartFd, 'E');

		XL_RETURN(ret < 0, -1, "Set Serial CheckOut is CHECKOUTBIT_PARITY error!");
	}
	else if(eCheckoutBitStatus == CHECKOUTBIT_EMPTY)	/* 空校验 */
	{
	}
	else if(eCheckoutBitStatus == CHECKOUTBIT_LABLE)	/* 标记校验 */
	{
	}

	return (0);
}


/**********************************************************************
*  Function Name：Stop_Bit_Set
*  Function description：停止位
*  Function parameter: eStopBitStatus:    STOPBIT_1BIT---> 一位
*										  STOPBIT_2BIT--->  二位
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Stop_Bit_Set(void)
{
	int ret;
	
	if (eStopBitStatus == STOPBIT_1BIT)			/* 一位 */
	{
		ret = set_stopbit(UartFd, 1);

		XL_RETURN(ret < 0, -1, "Set Serial Stop Bit is STOPBIT_1BIT error!");
	}
	else if(eStopBitStatus == STOPBIT_2BIT)		/* 二位 */
	{
		ret = set_stopbit(UartFd, 2);

		XL_RETURN(ret < 0, -1, "Set Serial Stop Bit is STOPBIT_2BIT error!");
	}

	return (0);
}

/**********************************************************************
*  Function Name：Terminator_Func_Set
*  Function description：结束符
*  Function parameter: eTerminatorStatus: TERMINATOR_NONE---> NONE 
*										  TERMINATOR_TAB--->  TAB(OD OA)
*										  TERMINATOR_ENTER-->ENTER(OD)
*										  TERMINATOR_SPACE--> SPACE(20)
*										  TERMINATOR_RETURN--> RETURN(OA)
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Terminator_Func_Set(Terminator eTerminatorStatus)
{
	if (eTerminatorStatus == TERMINATOR_NONE)		/* NONE */
	{
	}
	else if(eTerminatorStatus == TERMINATOR_TAB)	/* TAB(OD OA) */
	{
	}
	else if(eTerminatorStatus == TERMINATOR_ENTER)	/* ENTER(OD) */
	{
	}
	else if(eTerminatorStatus == TERMINATOR_SPACE)	/* SPACE(20) */
	{
	}
	else if(eTerminatorStatus == TERMINATOR_RETURN)	/* RETURN(OA) */
	{
	}

	return (0);
}


/**********************************************************************
*  Function Name：Scan_Mode_Set
*  Function description：扫描方式
*  Function parameter: eScanModeStatus: SCANMODE_TEST--->   测试模式 
*										SCANMODE_MANUAL---> 手动扫描
*										SCANMODE_LIGHT-->   连续常亮扫描
*										SCANMODE_TWINKLE--> 连续闪烁扫描
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Scan_Mode_Set(ScanMode eScanModeStatus)
{
	if (eScanModeStatus == SCANMODE_TEST)			/* 测试模式 */
	{
	}
	else if(eScanModeStatus == SCANMODE_MANUAL)		/* 手动扫描 */
	{
	}
	else if(eScanModeStatus == SCANMODE_LIGHT)		/* 连续常亮扫描 */
	{
	}
	else if(eScanModeStatus == SCANMODE_TWINKLE)	/* 连续闪烁扫描 */
	{
	}
	
	return (0);
}

/**********************************************************************
*  Function Name：Redund_Adjust_Set
*  Function description：冗余裁决
*  Function parameter: eRedundAdjustStatus: REDUNDADJUST_ONE--->   一次  
*										REDUNDADJUST_TWO--->   二次
*										REDUNDADJUST_THREE-->  三次
*										REDUNDADJUST_FOUR-->   四次
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Redund_Adjust_Set(RedundAdjust eRedundAdjustStatus)
{
	if (eRedundAdjustStatus == REDUNDADJUST_ONE)		/* 一次 */
	{
	}
	else if(eRedundAdjustStatus == REDUNDADJUST_TWO)	/* 二次 */
	{
	}
	else if(eRedundAdjustStatus == REDUNDADJUST_THREE)	/* 三次 */
	{
	}
	else if(eRedundAdjustStatus == REDUNDADJUST_FOUR)	/* 四次 */
	{
	}

	return (0);
}


/**********************************************************************
*  Function Name：Sound_Length_Set
*  Function description：音长
*  Function parameter: eSoundLengthStatus: 	SOUNDLENGTH_SHORT---> 	  短音  
*											SOUNDLENGTH_MIDDLE--->	  中音
*											SOUNDLENGTH_LONG-->   	  长音
*											SOUNDLENGTH_USERDEFINE--> 自定义
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Sound_Length_Set(SoundLength eSoundLengthStatus)
{
	if (eSoundLengthStatus == SOUNDLENGTH_SHORT)			/* 短音 */
	{
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_MIDDLE)		/* 中音 */
	{
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_LONG)			/* 长音 */
	{
	}
	else if(eSoundLengthStatus == SOUNDLENGTH_USERDEFINE)	/* 自定义 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Audio_Frequency_Set
*  Function description：音频
*  Function parameter: eAudioFrequencyStatus: AUDIOFREQUENCY_LOW---> 	  		低频  
*											  AUDIOFREQUENCY_INTERMEDIATE--->	中频
*											  AUDIOFREQUENCY_HIGH-->   	  		高频
*											  AUDIOFREQUENCY_USERDEFINE--> 		自定义
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Audio_Frequency_Set(AudioFrequency eAudioFrequencyStatus)
{
	if (eAudioFrequencyStatus == AUDIOFREQUENCY_LOW)				/* 低频 */
	{
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_INTERMEDIATE)	/* 中频 */
	{
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_HIGH)			/* 高频 */
	{
	}
	else if(eAudioFrequencyStatus == AUDIOFREQUENCY_USERDEFINE)		/* 自定义 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Volume_Function_Set
*  Function description：音量
*  Function parameter: eVolumeStatus: VOLUME_WHISPERING---> 	小声  
*									  VOLUME_MEDIUMSOUND--->	中声
*									  VOLUME_LOUD-->   	  		大声
*									  VOLUME_USERDEFINE--> 		自定义
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Volume_Function_Set(Volume eVolumeStatus)
{
	if (eVolumeStatus == VOLUME_WHISPERING)			/* 小声 */
	{
	}
	else if(eVolumeStatus == VOLUME_MEDIUMSOUND)	/* 中声 */
	{
	}
	else if(eVolumeStatus == VOLUME_LOUD)			/* 大声 */
	{
	}
	else if(eVolumeStatus == VOLUME_USERDEFINE)		/* 自定义 */
	{
	}

	return (0);
	
}

/**********************************************************************
*  Function Name：KB_Clock_Set
*  Function description：KB时钟
*  Function parameter: eKBclockStatus: KBCLOCK_10K---> 	10K  
*									  KBCLOCK_20K--->	20K
*									  KBCLOCK_30K--->   30K
*  return: success return 0
*		   error return  -1
**********************************************************************/
int KB_Clock_Set(KBclock eKBclockStatus)
{
	if (eKBclockStatus == KBCLOCK_10K)			/* 10K */
	{
	}
	else if(eKBclockStatus == KBCLOCK_20K)		/* 20K */
	{
	}
	else if(eKBclockStatus == KBCLOCK_30K)		/* 30K */
	{
	}
	
	return (0);
}

/**********************************************************************
*  Function Name：Data_Interception_Set
*  Function description：数据截取
*  Function parameter: eDataInterceptionStatus: DATAINTERCEPTION_NO--->不截取  
*									  			DATAINTERCEPTION_LEFT->	从左
*									  			DATAINTERCEPTION_RIGHT->从右
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Data_Interception_Set(DataInterception eDataInterceptionStatus)
{
	if (eDataInterceptionStatus == DATAINTERCEPTION_NO)			/* 不截取 */
	{
	}
	else if(eDataInterceptionStatus == DATAINTERCEPTION_LEFT)	/* 从左 */
	{
	}
	else if(eDataInterceptionStatus == DATAINTERCEPTION_RIGHT)	/* 从右 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：One_ReverseColor_Set
*  Function description：一维反色
*  Function parameter: eOneReverseColorStatus: ONEREADONLYBLACKCODE--->1d只读黑色码  
*									  		   ONEREADONLYREVERSECODE->1d只读反色码
*									  		   ONEBOTHREAD->1d正反都读
*  return: success return 0
*		   error return  -1
**********************************************************************/
int One_ReverseColor_Set(OneReverseColor eOneReverseColorStatus)
{
	if (eOneReverseColorStatus == ONEREADONLYBLACKCODE)			/* 1d只读黑色码 */
	{
	}
	else if(eOneReverseColorStatus == ONEREADONLYREVERSECODE)	/* 1d只读反色码 */
	{
	}
	else if(eOneReverseColorStatus == ONEBOTHREAD)				/* 1d正反都读 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Two_ReverseColor_Set
*  Function description：二维反色
*  Function parameter: eTwoReverseColorStatus: TWOREADONLYBLACKCODE--->2d只读黑色码  
*									  		   TWOREADONLYREVERSECODE->2d只读反色码
*									  		   TWOBOTHREAD->2d正反都读
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Two_ReverseColor_Set(TwoReverseColor eTwoReverseColorStatus)
{
	if (eTwoReverseColorStatus == TWOREADONLYBLACKCODE)			/* 2d只读黑色码 */
	{
	}
	else if(eTwoReverseColorStatus == TWOREADONLYREVERSECODE)	/* 2d只读反色码 */
	{
	}
	else if(eTwoReverseColorStatus == TWOBOTHREAD)				/* 2d正反都读 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：One_ReadMore_Set
*  Function description：一维多读
*  Function parameter: eOneReadMoreStatus: ONEREAdMORE_CLOSE--->一维多个读取关闭  
*									  	   ONEREAdMORE_OPEN->一维多个读取打开
*  return: success return 0
*		   error return  -1
**********************************************************************/
int One_ReadMore_Set(OneReadMore eOneReadMoreStatus)
{
	if (eOneReadMoreStatus == ONEREAdMORE_CLOSE)			/* 一维多个读取关闭 */
	{
	}
	else if(eOneReadMoreStatus == ONEREAdMORE_OPEN)			/* 一维多个读取打开 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Two_ReadMore_Set
*  Function description：二维多读
*  Function parameter: eTwoReadMoreChoice: TWOREADMORE_CLOSE--->二维多个读取关闭  
*									  	   TWOREADMORE_OPEN->二维多个读取打开
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Two_ReadMore_Set(TwoReadMore eTwoReadMoreStatus)
{
	if (eTwoReadMoreStatus == TWOREADMORE_CLOSE)			/* 二维多个读取关闭 */
	{
	}
	else if(eTwoReadMoreStatus == TWOREADMORE_OPEN)			/* 二维多个读取打开 */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Decode_Sleep_Set
*  Function description：解码休眠
*  Function parameter: eDecodeSleepStatus: DECODESLEEP_NON--->解码不休眠  
*									  	   DECODESLEEP_1S---->1s
*										   DECODESLEEP_1MIN-->1min
*										   DECODESLEEP_10MIN->10min
*										   DECODESLEEP_30MIN->30min
*										   DECODESLEEP_USER-->user
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Decode_Sleep_Set(DecodeSleep eDecodeSleepStatus)
{
	if (eDecodeSleepStatus == DECODESLEEP_NON)			/* 解码不休眠 */
	{
	}
	else if(eDecodeSleepStatus == DECODESLEEP_1S)		/* 1s */
	{
	}
	else if(eDecodeSleepStatus == DECODESLEEP_1MIN)		/* 1min */
	{
	}
	else if(eDecodeSleepStatus == DECODESLEEP_10MIN)	/* 10min */
	{
	}
	else if(eDecodeSleepStatus == DECODESLEEP_30MIN)	/* 30min */
	{
	}
	else if(eDecodeSleepStatus == DECODESLEEP_USER)		/* user */
	{
	}

	return (0);
	
}

/**********************************************************************
*  Function Name：Engine_Sleep_Set
*  Function description：引擎休眠
*  Function parameter: eEngineSleepStatus: ENGINESLEEP_NON--->引擎不休眠  
*									  	   ENGINESLEEP_IMMEDIATELY---->引擎立即休眠
*										   ENGINESLEEP_1MIN-->1min
*										   ENGINESLEEP_10MIN->10min
*										   ENGINESLEEP_30MIN->30min
*										   ENGINESLEEP_USER-->user
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Engine_Sleep_Set(EngineSleep eEngineSleepStatus)
{
	if (eEngineSleepStatus == ENGINESLEEP_NON)				/* 引擎不休眠 */
	{
	}
	else if(eEngineSleepStatus == ENGINESLEEP_IMMEDIATELY)	/* 引擎立即休眠 */
	{
	}
	else if(eEngineSleepStatus == ENGINESLEEP_1MIN)			/* 1min */
	{
	}
	else if(eEngineSleepStatus == ENGINESLEEP_10MIN)		/* 10min */
	{
	}
	else if(eEngineSleepStatus == ENGINESLEEP_30MIN)		/* 30min */
	{
	}
	else if(eEngineSleepStatus == ENGINESLEEP_USER)			/* user */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Read_Delay_Set
*  Function description：读取延时
*  Function parameter: eReadDelayStatus: READDELAY_NOT--->激光/读取延时关  
*									  	   READDELAY_300MS---->300ms
*										   READDELAY_600MS-->600ms
*										   READDELAY_900MS->900ms
*										   READDELAY_USER-->user
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Read_Delay_Set(ReadDelay eReadDelayStatus)
{
	if (eReadDelayStatus == READDELAY_NOT)			/* 激光/读取延时关 */
	{
	}
	else if(eReadDelayStatus == READDELAY_300MS)	/* 300ms */
	{
	}
	else if(eReadDelayStatus == READDELAY_600MS)	/* 600ms */
	{
	}
	else if(eReadDelayStatus == READDELAY_900MS)	/* 900ms */
	{
	}
	else if(eReadDelayStatus == READDELAY_USER)		/* user */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：AdditionCode_RdDelay_Set
*  Function description：附加码读取延时
*  Function parameter: eAdditionCodeRdDelayStatus: ADDITIONCODERDDELAY_CLOSE-->附加码延时关  
*									  	  		   ADDITIONCODERDDELAY_100MS-->100ms
*										   		   ADDITIONCODERDDELAY_200MS-->200ms
*										           ADDITIONCODERDDELAY_400MS-->400ms
*  return: success return 0
*		   error return  -1
**********************************************************************/
int AdditionCode_RdDelay_Set(AdditionCodeRdDelay eAdditionCodeRdDelayStatus)
{
	if (eAdditionCodeRdDelayStatus == ADDITIONCODERDDELAY_CLOSE)		/* 附加码延时关 */
	{
	}
	else if(eAdditionCodeRdDelayStatus == ADDITIONCODERDDELAY_100MS)	/* 100ms */
	{
	}
	else if(eAdditionCodeRdDelayStatus == ADDITIONCODERDDELAY_200MS)	/* 200ms */
	{
	}
	else if(eAdditionCodeRdDelayStatus == ADDITIONCODERDDELAY_400MS)	/* 400ms */
	{
	}

	return (0);
}

/**********************************************************************
*  Function Name：Aux_Status_Set
*  Function description：AUX
*  Function parameter: eAuxStatusStatus: AXUSTATUS_CLOSE-->AUX关闭  
*									  	 AXUSTATUS_EMBEDDEDDATA-->嵌入数据
*										 AXUSTATUS_TRANS-->透传
*  return: success return 0
*		   error return  -1
**********************************************************************/
int Aux_Status_Set(AuxStatus eAuxStatusStatus)
{
	if (eAuxStatusStatus == AXUSTATUS_CLOSE)				/* AUX关闭 */
	{
	}
	else if(eAuxStatusStatus == AXUSTATUS_EMBEDDEDDATA)		/* 嵌入数据 */
	{
	}
	else if(eAuxStatusStatus == AXUSTATUS_TRANS)			/* 透传 */
	{
	}

	return (0);
}


int WriteBufToFile(void)
{
	FILE *fp = NULL;
	int i = 0;

	fp = fopen("./setcode", "w");
	XL_RETURN(fp == NULL, -1, "Open setcode file error!");

	for (i = 0; i < sizeof(SETTAB)/sizeof(SETTAB[0]); i++)
	{
		fprintf(fp, "%d ", SETTAB[i]);
	}

	fclose(fp);

	return (0);
}

int ReadBufFromFile(void)
{
	FILE *fp = NULL;
	int i;

	fp = fopen("./setcode", "r");
	XL_RETURN(fp == NULL, -1, "Open Setcode File Error!");

	for (i = 0; i < sizeof(SETTAB)/sizeof(SETTAB[0]); i++)
	{
		fscanf(fp, "%d ", &SETTAB[i]);
	}

	fclose(fp);

	return (0);
}

/*
   // C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
   // parameter(s): [OUT] pbDest - 输出缓冲区
   // [IN] pbSrc - 字符串
   // [IN] nLen - 16进制数的字节数(字符串的长度/2)
   // return value: 
   // remarks : 将字符串转化为16进制数
   */
long StrToHex(char *pbSrc, int nLen)
{
    char h1;
    char s1;
    int i;
    long  ret = 0;

    for (i=0; i<nLen; i++)
    {
        h1 = pbSrc[i];
        s1 = toupper(h1) - 0x30;
        if (s1 > 9) 
            s1 -= 7;
        ret *=16;
        ret +=s1;
    }
	
    return ret;
}

int Init_Setcode(void)
{
	//ReadBufFromFile();
     memset(SETTAB,1,sizeof(SETTAB));
    return 0;
}

void SetCode(long setcode)
{
    if ((setcode>>8) <= 0x5E)
    {
        SETTAB[0xff&(setcode>>8)] = setcode & 0x01;
    }
	else
	{
		switch ( setcode )
		{
			/* NAK握手 */
			case 0x06400:   		/* NAK握手关闭 */
				ST_NAK = (int)HANDSHAKE_CLOSE;
				NAKStatus = HANDSHAKE_CLOSE;
				break;
			case 0x06401:		/* NAK握手开启 */
				ST_NAK = (int)HANDSHAKE_OPEN;
				NAKStatus = HANDSHAKE_OPEN;
				break;
				
			/* 硬件控制流 */
			case 0x06500:		/* 控制流关闭 */
				ST_CTS = (int)HARDWARECONTROLFLOW_CLOSE;
				iHdflowStatus = HARDWARECONTROLFLOW_CLOSE;
				break;
			case 0x06501:		/* 控制流开启 */
				ST_CTS = (int)HARDWARECONTROLFLOW_OPEN;
				iHdflowStatus = HARDWARECONTROLFLOW_OPEN;
				break;
				
			/* XON握手 */
			case 0x06600:		/* 握手开启 */
				ST_XON = (int)XONHANDSHAKE_OPEN;
				iXONStatus = XONHANDSHAKE_OPEN;
				break;
			case 0x06601:		/* 握手关闭 */
				ST_XON = (int)XONHANDSHAKE_CLOSE;
				iXONStatus = XONHANDSHAKE_CLOSE;
				break;

			/* COM触发开关 */
			case 0x06700:		/* COM充许触发读取 */
				ST_COMM = (int)COM_READ_WRITE_ALLOWED;
				iComTriggerSwitchStatus = COM_READ_WRITE_ALLOWED;
				break;
			case 0x06701:		/* 不充许 */
				ST_COMM = (int)TRIGGER_UNLIMITED;
				iComTriggerSwitchStatus = TRIGGER_UNLIMITED;
				break;

			/* 设置控制权 */
			case 0x06800:		/* 无限制 */
				ST_COMOWN = (int)CONTROL_UNLIMITED;
				iSetCtrlRightStatus = CONTROL_UNLIMITED;
				break;
			case 0x06801:		/* 禁止条码设置 */
				ST_COMOWN = (int)BARCODE_SET_PROHIBITED;
				iSetCtrlRightStatus = BARCODE_SET_PROHIBITED;
				break;
			case 0x06802:		/* 禁止COM设置 */
				ST_COMOWN = (int)COM_SET_PROHIBITED;
				iSetCtrlRightStatus = COM_SET_PROHIBITED;
				break;

			/* 串口设置提示音 */
			case 0x06900:		/* 关闭 */
				ST_COMBEEP = (int)SERIALPORTSETVOICE_CLOSE;
				eSerialPortStatus = SERIALPORTSETVOICE_CLOSE;
				break;
			case 0x06901:		/* 开启 */
				ST_COMBEEP = (int)SERIALPORTSETVOICE_OPEN;
				eSerialPortStatus = SERIALPORTSETVOICE_OPEN;
				break;

			/* 功能键盘 */
			case 0x06A00:		/* 关闭 */
				ST_FUNKEY = (int)FUNCTIONALKEYBOARD_CLOSE;
				eFuncKeyStatus = FUNCTIONALKEYBOARD_CLOSE;
				break;
			case 0x06A01:		/* 开启 */
				ST_FUNKEY = (int)FUNCTIONALKEYBOARD_OPNE;
				eFuncKeyStatus = FUNCTIONALKEYBOARD_OPNE;
				break;

			/* 小键盘 */
			case 0x06B00:		/* 关闭 */
				ST_NUMLOCK = (int)KEYPAD_CLOSE;
				eFunckeypadStatus = KEYPAD_CLOSE;
				break;
			case 0x06B01:		/* 开启 */
				ST_NUMLOCK = (int)KEYPAD_OPEN;
				eFunckeypadStatus = KEYPAD_OPEN;
				break;

			/* 大小写忽略 */
			case 0x06C00:		/* 关闭 */
				ST_CAPIG = (int)CASEIGNORED_CLOSE;
				eCaseIgnoreStatus = CASEIGNORED_CLOSE;
				break;
			case 0x06C01:		/* 开启 */
				ST_CAPIG = (int)CASEIGNORED_OPEN;
				eCaseIgnoreStatus = CASEIGNORED_OPEN;
				break;

			/* AIM码 */
			case 0x06D00:		/* 关闭 */
				ST_AIM = (int)AIMCODE_CLOSE;
				eAimCodeSetStatus = AIMCODE_CLOSE;
				break;
			case 0x06D01:		/* 开启 */
				ST_AIM = (int)AIMCODE_OPEN;
				eAimCodeSetStatus = AIMCODE_OPEN;
				break;

			/* CODE ID */
			case 0x06E00:		/* 关闭 */
				ST_CID = (int)CODEID_CLOSE;
				eCodeIdSetStatus = CODEID_CLOSE;
				break;
			case 0x06E01:		/* 开启 */
				ST_CID = (int)CODEID_OPEN;
				eCodeIdSetStatus = CODEID_OPEN;
				break;

			/* Febraban */
			case 0x06F00:		/* 关闭 */
				ST_BZCODE = (int)FEBRABAN_CLOSE;
				eFebrabanFuncSetStatus = FEBRABAN_CLOSE;
				break;
			case 0x06F01:		/* 开启 */
				ST_BZCODE = (int)FEBRABAN_OPEN;
				eFebrabanFuncSetStatus = FEBRABAN_OPEN;
				break;

			/* 开机音 */
			case 0x07000:		/* 关闭 */
				ST_TIPS = (int)BOOTSOUND_CLOSE;
				eBootSoundSetStatus = BOOTSOUND_CLOSE;
				break;
			case 0x07001:		/* 开启 */
				ST_TIPS = (int)BOOTSOUND_OPEN;
				eBootSoundSetStatus = BOOTSOUND_OPEN;
				break;

			/* 数据倒置 */
			case 0x07100:		/* 关闭 */
				ST_RDBACK = (int)DATAINVERSION_CLOSE;
				eDecodeDataInversionStatus = DATAINVERSION_CLOSE;
				break;
			case 0x07101:		/* 开启 */
				ST_RDBACK = (int)DATAINVERSION_OPEN;
				eDecodeDataInversionStatus = DATAINVERSION_OPEN;
				break;

			/* 长度前缀 */
			case 0x07200:		/* 关闭 */
				ST_LENGTH = (int)LENGTHPREFIX_CLOSE;
				eLengthPrefixSetStatus = LENGTHPREFIX_CLOSE;
				break;
			case 0x07201:		/* 开启 */
				ST_LENGTH = (int)LENGTHPROFIX_OPEN;
				eLengthPrefixSetStatus = LENGTHPROFIX_OPEN;
				break;

			/* 响铃传输 */
			case 0x07300:		/* 关闭 */
				ST_BELFIRST = (int)BELLTRANSMITION_CLOSE;
				eBellTransmitStatus = BELLTRANSMITION_CLOSE;
				break;
			case 0x07301:		/* 开启 */
				ST_BELFIRST = (int)BELLTRANSMITION_OPEN;
				eBellTransmitStatus = BELLTRANSMITION_OPEN;
				break;

			/* BEL功能 */
			case 0x07400:		/* 关闭 */
				ST_BEL07 = (int)BELFUNC_CLOSE;
				eBleFuncSetStatus = BELFUNC_CLOSE;
				break;
			case 0x07401:		/* 开启 */
				ST_BEL07 = (int)BELFUNC_OPEN;
				eBleFuncSetStatus = BELFUNC_OPEN;
				break;

			/* 休眠提示音 */
			case 0x07500:		/* 关闭 */
				ST_SLEEPBEEP = (int)SLEEPTIPVIOCE_CLOSE;
				eSleepTipVioceStatus = SLEEPTIPVIOCE_CLOSE;
				break;
			case 0x07501:		/* 开启 */
				ST_SLEEPBEEP = (int)SLEEPTIPVIOCE_OPEN;
				eSleepTipVioceStatus = SLEEPTIPVIOCE_OPEN;
				break;

			/* tropitic39 */
			case 0x07600:		/* 关闭 */
				ST_TROPITIC39 = (int)TROPITIC_CLOSE;
				eTropiticStatus = TROPITIC_CLOSE;
				break;
			case 0x07601:		/* 开启 */
				ST_TROPITIC39 = (int)TROPITIC_OPEN;
				eTropiticStatus = TROPITIC_OPEN;
				break;

			/* 自动感应 */
			case 0x07700:		/* 关闭 */
				ST_IRON = (int)AUTOMATICINDUCTION_CLOSE;
				eAutomaticInductionStatus = AUTOMATICINDUCTION_CLOSE;
				break;
			case 0x07701:		/* 开启 */
				ST_IRON = (int)AUTOMATICINDUCTION_OPEN;
				eAutomaticInductionStatus = AUTOMATICINDUCTION_OPEN;
				break;

			/* EAN13加强 */
			case 0x07800:		/* 关闭 */
				ST_EANDEC = (int)EANSTRENGTHEN_CLOSE;
				eEanStrenthenSetStatus = EANSTRENGTHEN_CLOSE;
				break;
			case 0x07801:		/* 开启 */
				ST_EANDEC = (int)EANSTRENGTHEN_OPEN;
				eEanStrenthenSetStatus = EANSTRENGTHEN_OPEN;
				break;

			/* ISBT */
			case 0x07900:		/* 关闭 */
				ST_ISBT = (int)ISBT_CLOSE;
				eIsbtStatusSetStatus = ISBT_CLOSE;
				break;
			case 0x07901:		/* 开启 */
				ST_ISBT = (int)ISBT_OPEN;
				eIsbtStatusSetStatus = ISBT_OPEN;
				break;

			/* LED提示 */
			case 0x07A00:		/* 关闭 */
				ST_LED = (int)LEDTIPS_CLOSE;
				eLedTipsStatus = LEDTIPS_CLOSE;
				break;
			case 0x07A01:		/* 开启 */
				ST_LED = (int)LEDTIPS_OPEN;
				eLedTipsStatus = LEDTIPS_OPEN;
				break;

			/* 相同条码读取 */
			case 0x07B00:		/* 关闭 */
				ST_SAMEREAD = (int)READSAMECODE_CLOSE;
				eRedSameCodeStatus = READSAMECODE_CLOSE;
				break;
			case 0x07B01:		/* 开启 */
				ST_SAMEREAD = (int)READSAMECODE_OPEN;
				eRedSameCodeStatus = READSAMECODE_OPEN;
				break;

			/* 读取预设 */
			case 0x07C00:		/* 自动 */
				ST_MONITER_ON = (int)READpresupposition_AUTO;
				eReadPresuppositionStatus = READpresupposition_AUTO;
				break;
			case 0x07C01:		/* 显示器 */
				ST_MONITER_ON = (int)READpresupposition_MONITOR;
				eReadPresuppositionStatus = READpresupposition_MONITOR;
				break;
			case 0x07C02:		/* 纸张 */
				ST_MONITER_ON = (int)READpresupposition_PAPER;
				eReadPresuppositionStatus = READpresupposition_PAPER;
				break;

			/* 补光灯 */
			case 0x07D00:		/* 关闭 */
				ST_LED_LUX = (int)FILLINLIGHT_CLOSE;
				eFillinLightSetStatus = FILLINLIGHT_CLOSE;
				break;
			case 0x07D01:		/* 低亮度 */
				ST_LED_LUX = (int)FILLINLIGHT_LOWBRIGHTNESS;
				eFillinLightSetStatus = FILLINLIGHT_LOWBRIGHTNESS;
				break;
			case 0x07D02:		/* 中亮度 */
				ST_LED_LUX = (int)FILLINLIGHT_MEDIUMBRIGHTNESS;
				eFillinLightSetStatus = FILLINLIGHT_MEDIUMBRIGHTNESS;
				break;
			case 0x07D03:		/* 高亮度 */
				ST_LED_LUX = (int)FILLINLIGHT_HIGHBRIGHTNESS;
				eFillinLightSetStatus = FILLINLIGHT_HIGHBRIGHTNESS;
				break;

			/* 标示灯 */
			case 0x07E00:		/* 标示光关 */
				ST_TAR_ON = (int)MARKLAMP_CLOSE;
				eMarkLampStatus = MARKLAMP_CLOSE;
				break;
			case 0x07E01:		/* 标示光开 */
				ST_TAR_ON = (int)MARKLAMP_OPEN;
				eMarkLampStatus = MARKLAMP_OPEN;
				break;

			/* 命令回应 */
			case 0x07F00:		/* 串口命令无返回 */
				ST_COM_ACK = (int)SERIAL_COMMAND_NO_RETURN;
				eCommandEchoStatus = SERIAL_COMMAND_NO_RETURN;
				break;
			case 0x07F01:		/* 串口命令返回 */
				ST_COM_ACK = (int)SERIAL_COMMAND_RETURN;
				eCommandEchoStatus = SERIAL_COMMAND_RETURN;
				break;

			/* 接口 */
			case 0x08200:		/* 自动 */
				ST_PORT = (int)INTERFACE_AUTO;
				eInterfaceStatus = INTERFACE_AUTO;
				break;
			case 0x08201:		/* KB */
				ST_PORT = (int)INTERFACE_KB;
				eInterfaceStatus = INTERFACE_KB;
				break;
			case 0x08202:		/* HID */
				ST_PORT = (int)INTERFACE_HID;
				eInterfaceStatus = INTERFACE_HID;
				break;
			case 0x08203:		/*  */
				//ST_PORT = (int)INTERFACE_HID;
				break;
			case 0x08204:		/* VCP */
				ST_PORT = (int)INTERFACE_VCP;
				eInterfaceStatus = INTERFACE_VCP;
				break;

			/* 语言 */
			case 0x08300:		/* 通用 */
				ST_LANG = (int)LANGUAGE_COMMON;
				eLanguageStatus = LANGUAGE_COMMON;
				break;
			case 0x08301:		/* 美国 */
				ST_LANG = (int)LANGUAGE_USA;
				eLanguageStatus = LANGUAGE_USA;
				break;
			case 0x08302:		/* 法国 */
				ST_LANG = (int)LANGUAGE_FRANCE;
				eLanguageStatus = LANGUAGE_FRANCE;
				break;
			case 0x08303:		/* 德国 */
				ST_LANG = (int)LANGUAGE_GERMANY;
				eLanguageStatus = LANGUAGE_GERMANY;
				break;
			case 0x08304:		/* 英国 */
				ST_LANG = (int)LANGUAGE_BRITAIN;
				eLanguageStatus = LANGUAGE_BRITAIN;
				break;
			case 0x08305:		/* 土耳其 */
				ST_LANG = (int)LANGUAGE_TURKEY;
				eLanguageStatus = LANGUAGE_TURKEY;
				break;
			case 0x08306:		/* 丹麦 */
				ST_LANG = (int)LANGUAGE_DENMARK;
				eLanguageStatus = LANGUAGE_DENMARK;
				break;
			case 0x08307:		/* 日本 */
				ST_LANG = (int)LANGUAGE_JAPAN;
				eLanguageStatus = LANGUAGE_JAPAN;
				break;
			case 0x08308:		/* 西班牙 */
				ST_LANG = (int)LANGUAGE_SPAIN;
				eLanguageStatus = LANGUAGE_SPAIN;
				break;
			case 0x08309:		/* 意大利 */
				ST_LANG = (int)LANGUAGE_ITALY;
				eLanguageStatus = LANGUAGE_ITALY;
				break;
			case 0x0830A:		/* 杰克 */
				ST_LANG = (int)LANGUAGE_JACK;
				eLanguageStatus = LANGUAGE_JACK;
				break;
			case 0x0830B:		/* 斯诺伐克 */
				ST_LANG = (int)LANGUAGE_SNOVUCK;
				eLanguageStatus = LANGUAGE_SNOVUCK;
				break;

			/* 波特率 */
			case 0x08400:		/* 1200 */
				ST_BAUTR = (int)BAUDRATE_1200N;
				eBaudRateStatus = BAUDRATE_1200N;			
				BaudRate_Func_set();				
				break;
			case 0x08401:		/* 2400 */
				ST_BAUTR = (int)BAUDRATE_2400N;
				eBaudRateStatus = BAUDRATE_2400N;
			
				BaudRate_Func_set();
				break;
			case 0x08402:		/* 4800 */
				ST_BAUTR = (int)BAUDRATE_4800N;
				eBaudRateStatus = BAUDRATE_4800N;
				BaudRate_Func_set();
				break;
			case 0x08403:		/* 9600 */
				ST_BAUTR = (int)BAUDRATE_9600N;
				eBaudRateStatus = BAUDRATE_9600N;
				BaudRate_Func_set();
				break;
			case 0x08404:		/* 14400 */
				ST_BAUTR = (int)BAUDRATE_14400N;
				eBaudRateStatus = BAUDRATE_14400N;
				BaudRate_Func_set();
				break;
			case 0x08405:		/* 19200 */
				ST_BAUTR = (int)BAUDRATE_19200N;
				eBaudRateStatus = BAUDRATE_19200N;
				BaudRate_Func_set();
				break;
			case 0x08406:		/* 28800 */
				ST_BAUTR = (int)BAUDRATE_28800N;
				eBaudRateStatus = BAUDRATE_28800N;
				BaudRate_Func_set();
				break;
			case 0x08407:		/* 38400 */
				ST_BAUTR = (int)BAUDRATE_38400N;
				eBaudRateStatus = BAUDRATE_38400N;
				BaudRate_Func_set();
				break;
			case 0x08408:		/* 57600 */
				ST_BAUTR = (int)BAUDRATE_57600N;
				eBaudRateStatus = BAUDRATE_57600N;
				BaudRate_Func_set();
				break;
			case 0x08409:		/* 115200 */
				ST_BAUTR = (int)BAUDRATE_115200N;
				eBaudRateStatus = BAUDRATE_115200N;
				BaudRate_Func_set();
				break;

			/* 数据位 */
			case 0x08500:		/* 7位 */
				ST_DATA = (int)DATABIT_7BIT;
				eDataBitStatus = DATABIT_7BIT;
				Data_Bit_Set();
				break;
			case 0x08501:		/* 8位 */
				ST_DATA = (int)DATABIT_8BIT;
				eDataBitStatus = DATABIT_8BIT;
				Data_Bit_Set();
				break;

			/* 校验位 */
			case 0x08600:		/* 无 */
				ST_CHECK = (int)CHECKOUTBIT_NONE;
				eCheckoutBitStatus = CHECKOUTBIT_NONE;
				Checkout_Bit_Set();
				break;
			case 0x08601:		/* 奇校验 */
				ST_CHECK = (int)CHECKOUTBIT_ODD;
				eCheckoutBitStatus = CHECKOUTBIT_ODD;
				Checkout_Bit_Set();
				break;
			case 0x08602:		/* 偶校验 */
				ST_CHECK = (int)CHECKOUTBIT_PARITY;
				eCheckoutBitStatus = CHECKOUTBIT_PARITY;
				Checkout_Bit_Set();
				break;
			case 0x08603:		/* 空校验 */
				ST_CHECK = (int)CHECKOUTBIT_EMPTY;
				eCheckoutBitStatus = CHECKOUTBIT_EMPTY;
				Checkout_Bit_Set();
				break;
			case 0x08604:		/* 标记校验 */
				ST_CHECK = (int)CHECKOUTBIT_LABLE;
				eCheckoutBitStatus = CHECKOUTBIT_LABLE;
				Checkout_Bit_Set();
				break;

			/* 停止位 */
			case 0x08700:		/* 一位 */
				ST_STOP = (int)STOPBIT_1BIT;
				eStopBitStatus = STOPBIT_1BIT;
				Stop_Bit_Set();
				break;
			case 0x08701:		/* 二位 */
				ST_STOP = (int)STOPBIT_2BIT;
				eStopBitStatus = STOPBIT_2BIT;
				Stop_Bit_Set();
				break;

			/* 结束符 */
			case 0x08800:		/* NONE */
				ST_TERM = (int)TERMINATOR_NONE;
				eTerminatorStatus = TERMINATOR_NONE;
				break;
			case 0x08801:		/* TAB(OD OA) */
				ST_TERM = (int)TERMINATOR_TAB;
				eTerminatorStatus = TERMINATOR_TAB;
				break;
			case 0x08802:		/* ENTER(OD) */
				ST_TERM = (int)TERMINATOR_ENTER;
				eTerminatorStatus = TERMINATOR_ENTER;
				break;
			case 0x08803:		/* SPACE(20) */
				ST_TERM = (int)TERMINATOR_SPACE;
				eTerminatorStatus = TERMINATOR_SPACE;
				break;
			case 0x08804:		/* RETURN(OA) */
				ST_TERM = (int)TERMINATOR_RETURN;
				eTerminatorStatus = TERMINATOR_RETURN;
				break;

			/* 大小写 */
			case 0x08900:		/* 原数据 */
				ST_DTTYPE = (int)CASEANDCASE_RAWDATA;
				eCaseAndCaseStatus = CASEANDCASE_RAWDATA;
				break;
			case 0x08901:		/* 强制大写 */
				ST_DTTYPE = (int)CASEANDCASE_UPPERCASE;
				eCaseAndCaseStatus = CASEANDCASE_UPPERCASE;
				break;
			case 0x08902:		/* 强制小写 */
				ST_DTTYPE = (int)CASEANDCASE_LOWERCASE;
				eCaseAndCaseStatus = CASEANDCASE_LOWERCASE;
				break;
			case 0x08904:		/* 大小写互换 */
				ST_DTTYPE = (int)CASEANDCASE_UPPERLOWEREXCHANGE;
				eCaseAndCaseStatus = CASEANDCASE_UPPERLOWEREXCHANGE;
				break;

			/* 扫描方式 */
			case 0x08A00:		/* 测试模式 */
				ST_REMOD = (int)SCANMODE_TEST;
				eScanModeStatus = SCANMODE_TEST;
				break;
			case 0x08A01:		/* 手动扫描 */
				ST_REMOD = (int)SCANMODE_MANUAL;
				eScanModeStatus = SCANMODE_MANUAL;
				break;
			case 0x08A02:		/* 连续常亮扫描 */
				ST_REMOD = (int)SCANMODE_LIGHT;
				eScanModeStatus = SCANMODE_LIGHT;
				break;
			case 0x08A03:		/* 连续闪烁扫描 */
				ST_REMOD = (int)SCANMODE_TWINKLE;
				eScanModeStatus = SCANMODE_TWINKLE;
				break;

			/* 冗余裁决 */
			case 0x08B00:		/* 一次 */
				ST_REDUN = (int)REDUNDADJUST_ONE;
				eRedundAdjustStatus = REDUNDADJUST_ONE;
				break;
			case 0x08B01:		/* 二次 */
				ST_REDUN = (int)REDUNDADJUST_TWO;
				eRedundAdjustStatus = REDUNDADJUST_TWO;
				break;
			case 0x08B02:		/* 三次 */
				ST_REDUN = (int)REDUNDADJUST_THREE;
				eRedundAdjustStatus = REDUNDADJUST_THREE;
				break;
			case 0x08B03:		/* 四次 */
				ST_REDUN = (int)REDUNDADJUST_FOUR;
				eRedundAdjustStatus = REDUNDADJUST_FOUR;
				break;

			/* 音长 */
			case 0x08C00:		/* 短音 */
				ST_BPLONG = (int)SOUNDLENGTH_SHORT;
				eSoundLengthStatus = SOUNDLENGTH_SHORT;
				break;
			case 0x08C01:		/* 中音 */
				ST_BPLONG = (int)SOUNDLENGTH_MIDDLE;
				eSoundLengthStatus = SOUNDLENGTH_MIDDLE;
				break;
			case 0x08C02:		/* 长音 */
				ST_BPLONG = (int)SOUNDLENGTH_LONG;
				eSoundLengthStatus = SOUNDLENGTH_LONG;
				break;
			case 0x08C03:		/* 自定义 */
				ST_BPLONG = (int)SOUNDLENGTH_USERDEFINE;
				eSoundLengthStatus = SOUNDLENGTH_USERDEFINE;
				break;

			/* 音频 */
			case 0x08D00:		/* 低频 */
				ST_FREQ = (int)AUDIOFREQUENCY_LOW;
				eAudioFrequencyStatus = AUDIOFREQUENCY_LOW;
				break;
			case 0x08D01:		/* 中频 */
				ST_FREQ = (int)AUDIOFREQUENCY_INTERMEDIATE;
				eAudioFrequencyStatus = AUDIOFREQUENCY_INTERMEDIATE;
				break;
			case 0x08D02:		/* 高频 */
				ST_FREQ = (int)AUDIOFREQUENCY_HIGH;
				eAudioFrequencyStatus = AUDIOFREQUENCY_HIGH;
				break;
			case 0x08D03:		/* 自定义 */
				ST_FREQ = (int)AUDIOFREQUENCY_USERDEFINE;
				eAudioFrequencyStatus = AUDIOFREQUENCY_USERDEFINE;
				break;

			/* 音量 */
			case 0x08E00:		/* 小声 */
				ST_SOUNDLEV = (int)VOLUME_WHISPERING;
				eVolumeStatus = VOLUME_WHISPERING;
				break;
			case 0x08E01:		/* 中声 */
				ST_SOUNDLEV = (int)VOLUME_MEDIUMSOUND;
				eVolumeStatus = VOLUME_MEDIUMSOUND;
				break;
			case 0x08E02:		/* 大声 */
				ST_SOUNDLEV = (int)VOLUME_LOUD;
				eVolumeStatus = VOLUME_LOUD;
				break;
			case 0x08E03:		/* 自定义 */
				ST_SOUNDLEV = (int)VOLUME_USERDEFINE;
				eVolumeStatus = VOLUME_USERDEFINE;
				break;

			/* KB时钟 */
			case 0x08F00:		/* 10K */
				ST_CLK = (int)KBCLOCK_10K;
				eKBclockStatus = KBCLOCK_10K;
				break;
			case 0x08F01:		/* 20K */
				ST_CLK = (int)KBCLOCK_20K;
				eKBclockStatus = KBCLOCK_20K;
				break;
			case 0x08F02:		/* 30K */
				ST_CLK = (int)KBCLOCK_30K;
				eKBclockStatus = KBCLOCK_30K;
				break;

			/* 数据截取 */
			case 0x09000:		/* 不截取 */
				ST_DRER = (int)DATAINTERCEPTION_NO;
				eDataInterceptionStatus = DATAINTERCEPTION_NO;
				break;
			case 0x09001:		/* 从左 */
				ST_DRER = (int)DATAINTERCEPTION_LEFT;
				eDataInterceptionStatus = DATAINTERCEPTION_LEFT;
				break;
			case 0x09002:		/* 从右 */
				ST_DRER = (int)DATAINTERCEPTION_RIGHT;
				eDataInterceptionStatus = DATAINTERCEPTION_RIGHT;
				break;

			/* 一维反色 */
			case 0x09100:		/* 1d只读黑色码 */
				ST_RESERV1D = (int)ONEREADONLYBLACKCODE;
				eOneReverseColorStatus = ONEREADONLYBLACKCODE;
				break;
			case 0x09101:		/* 1d只读反色码 */
				ST_RESERV1D = (int)ONEREADONLYREVERSECODE;
				eOneReverseColorStatus = ONEREADONLYREVERSECODE;
				break;
			case 0x09102:		/* 1d正反都读 */
				ST_RESERV1D = (int)ONEBOTHREAD;
				eOneReverseColorStatus = ONEBOTHREAD;
				break;

			/* 二维反色 */
			case 0x09200:		/* 2d只读黑色码 */
				ST_RESERV2D = (int)TWOREADONLYBLACKCODE;
				eTwoReverseColorStatus = TWOREADONLYBLACKCODE;
				break;
			case 0x09201:		/* 2d只读反色码 */
				ST_RESERV2D = (int)TWOREADONLYREVERSECODE;
				eTwoReverseColorStatus = TWOREADONLYREVERSECODE;
				break;
			case 0x09202:		/* 2d正反都读 */
				ST_RESERV2D = (int)TWOBOTHREAD;
				eTwoReverseColorStatus = TWOBOTHREAD;
				break;

			/* 一维多读 */
			case 0x09300:		/* 一维多个读取关闭 */
				ST_APPEND1D = (int)ONEREAdMORE_CLOSE;
				eOneReadMoreStatus = ONEREAdMORE_CLOSE;
				break;
			case 0x09301:		/* 一维多个读取打开 */
				ST_APPEND1D = (int)ONEREAdMORE_OPEN;
				eOneReadMoreStatus = ONEREAdMORE_OPEN;
				break;

			/* 二维多读 */
			case 0x09400:		/* 二维多个读取关闭 */
				ST_APPEND2D = (int)TWOREADMORE_CLOSE;
				eTwoReadMoreStatus = TWOREADMORE_CLOSE;
				break;
			case 0x09401:		/* 二维多个读取打开 */
				ST_APPEND2D = (int)TWOREADMORE_OPEN;
				eTwoReadMoreStatus = TWOREADMORE_OPEN;
				break;

			/* 解码休眠 */
			case 0x09500:		/* 解码不休眠 */
				ST_DCSLEEP = (int)DECODESLEEP_NON;
				eDecodeSleepStatus = DECODESLEEP_NON;
				break;
			case 0x09501:		/* 1s */
				ST_DCSLEEP = (int)DECODESLEEP_1S;
				eDecodeSleepStatus = DECODESLEEP_1S;
				break;
			case 0x09502:		/* 1min */
				ST_DCSLEEP = (int)DECODESLEEP_1MIN;
				eDecodeSleepStatus = DECODESLEEP_1MIN;
				break;
			case 0x09503:		/* 10min */
				ST_DCSLEEP = (int)DECODESLEEP_10MIN;
				eDecodeSleepStatus = DECODESLEEP_10MIN;
				break;
			case 0x09504:		/* 30min */
				ST_DCSLEEP = (int)DECODESLEEP_30MIN;
				eDecodeSleepStatus = DECODESLEEP_30MIN;
				break;
			case 0x09505:		/* user */
				ST_DCSLEEP = (int)DECODESLEEP_USER;
				eDecodeSleepStatus = DECODESLEEP_USER;
				break;

			/* 引擎休眠 */
			case 0x09600:		/* 引擎不休眠 */
				ST_ENSLEEP = (int)ENGINESLEEP_NON;
				eEngineSleepStatus = ENGINESLEEP_NON;
				break;
			case 0x09601:		/* 立即休眠 */
				ST_ENSLEEP = (int)ENGINESLEEP_IMMEDIATELY;
				eEngineSleepStatus = ENGINESLEEP_IMMEDIATELY;
				break;
			case 0x09602:		/* 1min */
				ST_ENSLEEP = (int)ENGINESLEEP_1MIN;
				eEngineSleepStatus = ENGINESLEEP_1MIN;
				break;
			case 0x09603:		/* 10min */
				ST_ENSLEEP = (int)ENGINESLEEP_10MIN;
				eEngineSleepStatus = ENGINESLEEP_10MIN;
				break;
			case 0x09604:		/* 30min */
				ST_ENSLEEP = (int)ENGINESLEEP_30MIN;
				eEngineSleepStatus = ENGINESLEEP_30MIN;
				break;
			case 0x09605:		/* user */
				ST_ENSLEEP = (int)ENGINESLEEP_USER;
				eEngineSleepStatus = ENGINESLEEP_USER;
				break;

			/* 读取延时 */
			case 0x09700:		/* 激光/读取延时关 */
				ST_LAYSERDELAY = (int)READDELAY_NOT;
				eReadDelayStatus = READDELAY_NOT;
				break;	
			case 0x09701:		/* 300ms */
				ST_LAYSERDELAY = (int)READDELAY_300MS;
				eReadDelayStatus = READDELAY_300MS;
				break;
			case 0x09702:		/* 600ms */
				ST_LAYSERDELAY = (int)READDELAY_600MS;
				eReadDelayStatus = READDELAY_600MS;
				break;
			case 0x09703:		/* 900ms*/
				ST_LAYSERDELAY = (int)READDELAY_900MS;
				eReadDelayStatus = READDELAY_900MS;
				break;
			case 0x09704:		/* user */
				ST_LAYSERDELAY = (int)READDELAY_USER;
				eReadDelayStatus = READDELAY_USER;
				break;

			/* 附加码读取延时 */
			case 0x09800:		/* 附加码延时关 */
				ST_ADD_DELAY = (int)ADDITIONCODERDDELAY_CLOSE;
				eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_CLOSE;
				break;
			case 0x09801:		/* 100ms */
				ST_ADD_DELAY = (int)ADDITIONCODERDDELAY_100MS;
				eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_100MS;
				break;
			case 0x09802:		/* 200ms */
				ST_ADD_DELAY = (int)ADDITIONCODERDDELAY_200MS;
				eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_200MS;
				break;
			case 0x09803:		/* 400ms */
				ST_ADD_DELAY = (int)ADDITIONCODERDDELAY_400MS;
				eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_400MS;
				break;

			/* AUX */
			case 0x09900:		/* AUX关闭 */
				ST_AUX_MOD = (int)AXUSTATUS_CLOSE;
				eAuxStatusStatus = AXUSTATUS_CLOSE;
				break;
			case 0x09901:		/* 嵌入数据 */
				ST_AUX_MOD = (int)AXUSTATUS_EMBEDDEDDATA;
				eAuxStatusStatus = AXUSTATUS_EMBEDDEDDATA;
				break;
			case 0x09902:		/* 透传 */
				ST_AUX_MOD = (int)AXUSTATUS_TRANS;
				eAuxStatusStatus = AXUSTATUS_TRANS;
				break;

		}
	}

	WriteBufToFile();
}

int Setcode_Init(void)
{
	/* NAK握手 */
	if (ST_NAK == (int)HANDSHAKE_CLOSE)  		/* NAK握手关闭 */
	{	
		NAKStatus = HANDSHAKE_CLOSE;
	}
	else if(ST_NAK == (int)HANDSHAKE_OPEN)		/* NAK握手开启 */
	{
		NAKStatus = HANDSHAKE_OPEN;
	}

	/* 硬件控制流 */
	if (ST_CTS == (int)HARDWARECONTROLFLOW_CLOSE)	/* 控制流关闭 */
	{
		iHdflowStatus = HARDWARECONTROLFLOW_CLOSE;
	}
	else if(ST_CTS == (int)HARDWARECONTROLFLOW_OPEN) /* 控制流开启 */
	{
		iHdflowStatus = HARDWARECONTROLFLOW_OPEN;
	}

	/* XON握手 */
	if (ST_XON == (int)XONHANDSHAKE_OPEN)			/* XON握手开启 */
	{
		iXONStatus = XONHANDSHAKE_OPEN;
	}
	else if(ST_XON == (int)XONHANDSHAKE_CLOSE)		/* XON握手关闭 */
	{
		iXONStatus = XONHANDSHAKE_CLOSE;
	}

	/* COM触发开关 */
	if (ST_COMM == (int)COM_READ_WRITE_ALLOWED)		/* COM充许触发读写 */
	{
		iComTriggerSwitchStatus = COM_READ_WRITE_ALLOWED;
	}
	else if(ST_COMM == (int)TRIGGER_UNLIMITED)		/* 无限制 */
	{
		iComTriggerSwitchStatus = TRIGGER_UNLIMITED;
	}

	/* 设置控制权 */
	if (ST_COMOWN == (int)CONTROL_UNLIMITED)			/* 无限制 */	
	{
		iSetCtrlRightStatus = CONTROL_UNLIMITED;
	}
	else if(ST_COMOWN == (int)BARCODE_SET_PROHIBITED)/* 禁止条码设置 */
	{
		iSetCtrlRightStatus = BARCODE_SET_PROHIBITED;
	}
	else if(ST_COMOWN == (int)COM_SET_PROHIBITED)	/* 禁止COM设置 */
	{
		iSetCtrlRightStatus = COM_SET_PROHIBITED;
	}

	/* 串口设置提示音 */
	if (ST_COMBEEP == (int)SERIALPORTSETVOICE_CLOSE)	/* 串口设置提示音关闭 */
	{
		eSerialPortStatus = SERIALPORTSETVOICE_CLOSE;
	}
	else if(ST_COMBEEP == (int)SERIALPORTSETVOICE_OPEN)/* 串口设置提示音开启 */
	{
		eSerialPortStatus = SERIALPORTSETVOICE_OPEN;
	}

	/* 功能键盘 */
	if (ST_FUNKEY == (int)FUNCTIONALKEYBOARD_CLOSE)	/* 功能键盘关闭 */
	{
		eFuncKeyStatus = FUNCTIONALKEYBOARD_CLOSE;
	}
	else if(ST_FUNKEY == (int)FUNCTIONALKEYBOARD_OPNE)	/* 功能键盘开启 */
	{
		eFuncKeyStatus = FUNCTIONALKEYBOARD_OPNE;
	}

	/* 小键盘 */
	if (ST_NUMLOCK == (int)KEYPAD_CLOSE)				/* 小键盘关闭 */
	{
		eFunckeypadStatus = KEYPAD_CLOSE;
	}
	else if(ST_NUMLOCK == (int)KEYPAD_OPEN)			/* 小键盘开启 */
	{
		eFunckeypadStatus = KEYPAD_OPEN;
	}

	/* 大小写忽略 */
	if (ST_CAPIG == (int)CASEIGNORED_CLOSE)			/* 大小写忽略关闭 */
	{
		eCaseIgnoreStatus = CASEIGNORED_CLOSE;
	}
	else if(ST_CAPIG == (int)CASEIGNORED_OPEN)		/* 大小写忽略开启 */
	{
		eCaseIgnoreStatus = CASEIGNORED_OPEN;
	}

	/* AIM码 */
	if (ST_AIM == (int)AIMCODE_CLOSE)				/* AIM码关闭 */
	{
		eAimCodeSetStatus = AIMCODE_CLOSE;
	}
	else if(ST_AIM == (int)AIMCODE_OPEN)				/* AIM码开启 */
	{
		eAimCodeSetStatus = AIMCODE_OPEN;
	}

	/* CODE ID */
	if (ST_CID == (int)CODEID_CLOSE)					/* CODE ID 关闭 */
	{
		eCodeIdSetStatus = CODEID_CLOSE;
	}
	else if(ST_CID == (int)CODEID_OPEN)				/* CODE ID 开启 */
	{
		eCodeIdSetStatus = CODEID_OPEN;
	}

	/* Febraban */
	if (ST_BZCODE == (int)FEBRABAN_CLOSE)			/* Febraban 关闭 */
	{
		eFebrabanFuncSetStatus = FEBRABAN_CLOSE;
	}
	else if(ST_BZCODE == (int)FEBRABAN_OPEN)			/* Febraban 开启 */
	{
		eFebrabanFuncSetStatus = FEBRABAN_OPEN;
	}

	/* 开机音 */
	if (ST_TIPS == (int)BOOTSOUND_CLOSE)				/* 开机音关闭 */
	{
		eBootSoundSetStatus = BOOTSOUND_CLOSE;
	}
	else if(ST_TIPS == (int)BOOTSOUND_OPEN)			/* 开机音开启 */
	{
		eBootSoundSetStatus = BOOTSOUND_OPEN;
	}

	/* 数据倒置 */
	if (ST_RDBACK == (int)DATAINVERSION_CLOSE)		/* 数据倒置关闭 */
	{
		eDecodeDataInversionStatus = DATAINVERSION_CLOSE;
	}
	else if(ST_RDBACK == (int)DATAINVERSION_OPEN)	/* 数据倒置开启 */
	{
		eDecodeDataInversionStatus = DATAINVERSION_OPEN;
	}

	/* 长度前缀 */
	if (ST_LENGTH == (int)LENGTHPREFIX_CLOSE)		/* 长度前缀关闭 */
	{
		eLengthPrefixSetStatus = LENGTHPREFIX_CLOSE;
	}
	else if(ST_LENGTH == (int)LENGTHPROFIX_OPEN)		/* 长度前缀开启 */
	{
		eLengthPrefixSetStatus =LENGTHPROFIX_OPEN;
	}

	/* 响铃传输 */
	if (ST_BELFIRST == (int)BELLTRANSMITION_CLOSE)		/* 响铃传输关闭 */
	{
		eBellTransmitStatus = BELLTRANSMITION_CLOSE;
	}
	else if(ST_BELFIRST == (int)BELLTRANSMITION_OPEN)	/* 响铃传输开启 */
	{
		eBellTransmitStatus = BELLTRANSMITION_OPEN;
	}

	/* BEL功能 */
	if (ST_BEL07 == (int)BELFUNC_CLOSE)					/* BEL功能关闭 */
	{
		eBleFuncSetStatus = BELFUNC_CLOSE;
	}
	else if(ST_BEL07 == (int)BELFUNC_OPEN)				/* BEL功能开启 */
	{
		eBleFuncSetStatus = BELFUNC_OPEN;
	}

	/* 休眠提示音 */
	if (ST_SLEEPBEEP == (int)SLEEPTIPVIOCE_CLOSE)		/* 休眠提示音关闭 */
	{
		eSleepTipVioceStatus = SLEEPTIPVIOCE_CLOSE;
	}
	else if(ST_SLEEPBEEP == (int)SLEEPTIPVIOCE_OPEN)		/* 休眠提示音开启 */
	{
		eSleepTipVioceStatus = SLEEPTIPVIOCE_OPEN;
	}

	/* tropitic39 */
	if (ST_TROPITIC39 == (int)TROPITIC_CLOSE)			/* tropitic39关闭 */
	{
		eTropiticStatus = TROPITIC_CLOSE;
	}
	else if(ST_TROPITIC39 == (int)TROPITIC_OPEN)			/* tropitic39开启 */
	{
		eTropiticStatus = TROPITIC_OPEN;
	}

	/* 自动感应 */
	if (ST_IRON == (int)AUTOMATICINDUCTION_CLOSE)		/* 自动感应关闭 */
	{
		eAutomaticInductionStatus = AUTOMATICINDUCTION_CLOSE;
	}
	else if(ST_IRON == (int)AUTOMATICINDUCTION_OPEN)		/* 自动感应开启 */
	{
		eAutomaticInductionStatus = AUTOMATICINDUCTION_OPEN;
	}

	/* EAN13 加强 */
	if (ST_EANDEC == (int)EANSTRENGTHEN_CLOSE)			/* EAN13加强关闭 */
	{
		eEanStrenthenSetStatus = EANSTRENGTHEN_CLOSE;
	}
	else if(ST_EANDEC == (int)EANSTRENGTHEN_OPEN)		/* EAN13加强开启 */
	{
		eEanStrenthenSetStatus = EANSTRENGTHEN_OPEN;
	}

	/* ISBT */
	if (ST_ISBT == (int)ISBT_CLOSE)						/* ISBT关闭 */
	{
		eIsbtStatusSetStatus = ISBT_CLOSE;
	}
	else if(ST_ISBT == (int)ISBT_OPEN)					/* ISBT开启 */
	{
		eIsbtStatusSetStatus = ISBT_OPEN;
	}

	/* LED提示 */
	if (ST_LED == (int)LEDTIPS_CLOSE)					/* LED提示关闭 */
	{
		eLedTipsStatus = LEDTIPS_CLOSE;
	}
	else if(ST_LED == (int)LEDTIPS_OPEN)				/* LED提示开启 */
	{
		eLedTipsStatus = LEDTIPS_OPEN;
	}

	/* 相同条码读取 */
	if (ST_SAMEREAD == (int)READSAMECODE_CLOSE)			/* 相同条码读取关闭 */
	{
		eRedSameCodeStatus = READSAMECODE_CLOSE;
	}
	else if(ST_SAMEREAD == (int)READSAMECODE_OPEN)		/* 相同条码读取开启 */	
	{
		eRedSameCodeStatus = READSAMECODE_OPEN;
	}

	/* 读取预设 */
	if (ST_MONITER_ON == (int)READpresupposition_AUTO)		/* 读取预设自动 */
	{
		eReadPresuppositionStatus = READpresupposition_AUTO;
	}
	else if(ST_MONITER_ON == (int)READpresupposition_MONITOR)	/* 读取预设显示器 */
	{
		eReadPresuppositionStatus = READpresupposition_MONITOR;
	}
	else if(ST_MONITER_ON == (int)READpresupposition_PAPER)		/* 读取预设纸张 */
	{
		eReadPresuppositionStatus = READpresupposition_PAPER;
	}

	/* 补光灯 */
	if (ST_LED_LUX == (int)FILLINLIGHT_CLOSE)					/* 补光灯关闭 */
	{
		eFillinLightSetStatus = FILLINLIGHT_CLOSE;
	}
	else if(ST_LED_LUX == (int)FILLINLIGHT_LOWBRIGHTNESS)		/* 补光灯低亮度 */
	{
		eFillinLightSetStatus = FILLINLIGHT_LOWBRIGHTNESS;
	}
	else if(ST_LED_LUX == (int)FILLINLIGHT_MEDIUMBRIGHTNESS)	/* 补光灯中亮度 */
	{
		eFillinLightSetStatus = FILLINLIGHT_MEDIUMBRIGHTNESS;
	}
	else if(ST_LED_LUX == (int)FILLINLIGHT_HIGHBRIGHTNESS)		/* 补光灯高亮度 */
	{
		eFillinLightSetStatus = FILLINLIGHT_HIGHBRIGHTNESS;
	}

	/* 标示灯 */
	if (ST_TAR_ON == (int)MARKLAMP_CLOSE)							/* 标示灯关 */
	{
		eMarkLampStatus = MARKLAMP_CLOSE;
	}
	else if(ST_TAR_ON == (int)MARKLAMP_OPEN)							/* 标示灯开 */
	{
		eMarkLampStatus = MARKLAMP_OPEN;
	}

	/* 命令回应 */
	if (ST_COM_ACK == (int)SERIAL_COMMAND_NO_RETURN)					/* 串口命令无返回 */
	{
		eCommandEchoStatus = SERIAL_COMMAND_NO_RETURN;
	}
	else if(ST_COM_ACK == (int)SERIAL_COMMAND_RETURN)				/* 串口命令有返回 */
	{
		eCommandEchoStatus = SERIAL_COMMAND_RETURN;
	}

	/* 接口 */
	if (ST_PORT == (int)INTERFACE_AUTO)								/* 自动 */
	{
		eInterfaceStatus = INTERFACE_AUTO;
	}
	else if(ST_PORT == (int)INTERFACE_KB)							/* KB */
	{
		eInterfaceStatus = INTERFACE_KB;
	}
	else if(ST_PORT == (int)INTERFACE_HID)							/* HID */
	{
		eInterfaceStatus = INTERFACE_HID;
	}
	else if(ST_PORT == (int)INTERFACE_VCP)							/* VCP */
	{
		eInterfaceStatus = INTERFACE_VCP;
	}

	/* 语言 */
	if (ST_LANG == (int)LANGUAGE_COMMON)					/* 通用 */
	{
		eLanguageStatus = LANGUAGE_COMMON;
	}
	else if(ST_LANG == (int)LANGUAGE_USA)				/* 美国 */
	{
		eLanguageStatus = LANGUAGE_USA;
	}
	else if(ST_LANG == (int)LANGUAGE_FRANCE)				/* 法国 */
	{
		eLanguageStatus = LANGUAGE_FRANCE;
	}
	else if(ST_LANG == (int)LANGUAGE_GERMANY)			/* 德国 */
	{
		eLanguageStatus = LANGUAGE_GERMANY;
	}
	else if(ST_LANG == (int)LANGUAGE_BRITAIN)			/* 英国 */
	{
		eLanguageStatus = LANGUAGE_BRITAIN;
	}
	else if(ST_LANG == (int)LANGUAGE_TURKEY)				/* 土耳其 */
	{
		eLanguageStatus = LANGUAGE_TURKEY;
	}
	else if(ST_LANG == (int)LANGUAGE_DENMARK)			/* 丹麦 */
	{
		eLanguageStatus = LANGUAGE_DENMARK;
	}
	else if(ST_LANG == (int)LANGUAGE_JAPAN)				/* 日本 */
	{
		eLanguageStatus = LANGUAGE_JAPAN;
	}
	else if(ST_LANG == (int)LANGUAGE_SPAIN)				/* 西班牙 */
	{
		eLanguageStatus = LANGUAGE_SPAIN;
	}
	else if(ST_LANG == (int)LANGUAGE_ITALY)				/* 意大利 */
	{
		eLanguageStatus = LANGUAGE_ITALY;
	}
	else if(ST_LANG == (int)LANGUAGE_JACK)				/* 杰克 */
	{
		eLanguageStatus = LANGUAGE_JACK;
	}
	else if(ST_LANG == (int)LANGUAGE_SNOVUCK)			/* 斯诺伐克 */
	{
		eLanguageStatus = LANGUAGE_SNOVUCK;
	}

	/* 波特率 */
	if (ST_BAUTR == (int)BAUDRATE_1200N)				/* 波特率1200 */
	{
		eBaudRateStatus = BAUDRATE_1200N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_2400N)			/* 波特率2400 */
	{
		eBaudRateStatus = BAUDRATE_2400N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_4800N)			/* 波特率4800 */
	{
		eBaudRateStatus = BAUDRATE_4800N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_9600N)			/* 波特率9600 */
	{
		eBaudRateStatus = BAUDRATE_9600N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_14400N)		/* 波特率14400 */	
	{
		eBaudRateStatus = BAUDRATE_14400N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_19200N)		/* 波特率19200 */
	{
		eBaudRateStatus = BAUDRATE_19200N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_28800N)		/* 波特率28800 */
	{
		eBaudRateStatus = BAUDRATE_28800N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_38400N)		/* 波特率38400 */
	{
		eBaudRateStatus = BAUDRATE_38400N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_57600N)		/* 波特率57600 */
	{
		eBaudRateStatus = BAUDRATE_57600N;
		
		BaudRate_Func_set();
	}
	else if(ST_BAUTR == (int)BAUDRATE_115200N)		/* 波特率115200 */
	{
		eBaudRateStatus = BAUDRATE_115200N;
		
		BaudRate_Func_set();
	}

	/* 数据位 */
	if (ST_DATA == (int)DATABIT_7BIT)				/* 7位 */
	{
		eDataBitStatus = DATABIT_7BIT;
		
		Data_Bit_Set();
	}
	else if(ST_DATA == (int)DATABIT_8BIT)			/* 8位 */
	{
		eDataBitStatus = DATABIT_8BIT;
		
		Data_Bit_Set();
	}

	/* 校验位 */
	if (ST_CHECK == (int)CHECKOUTBIT_NONE)			/* 无校验 */
	{
		eCheckoutBitStatus = CHECKOUTBIT_NONE;

		Checkout_Bit_Set();
	}
	else if(ST_CHECK == (int)CHECKOUTBIT_ODD)		/* 奇校验 */
	{
		eCheckoutBitStatus = CHECKOUTBIT_ODD;

		Checkout_Bit_Set();
	}
	else if(ST_CHECK == CHECKOUTBIT_PARITY)		/* 偶校验 */
	{
		eCheckoutBitStatus = CHECKOUTBIT_PARITY;

		Checkout_Bit_Set();
	}
	else if(ST_CHECK == (int)CHECKOUTBIT_EMPTY)		/* 空校验 */
	{
		eCheckoutBitStatus = CHECKOUTBIT_EMPTY;

		Checkout_Bit_Set();
	}
	else if(ST_CHECK == (int)CHECKOUTBIT_LABLE)		/* 标记校验 */
	{
		eCheckoutBitStatus = CHECKOUTBIT_LABLE;

		Checkout_Bit_Set();
	}

	/* 停止位 */
	if (ST_STOP == (int)STOPBIT_1BIT)				/* 一位 */
	{
		eStopBitStatus = STOPBIT_1BIT;
		
		Stop_Bit_Set();
	}
	else if(ST_STOP == (int)STOPBIT_2BIT)			/* 二位 */
	{
		eStopBitStatus = STOPBIT_2BIT;
		
		Stop_Bit_Set();
	}

	/* 结束符 */
	if (ST_TERM == (int)TERMINATOR_NONE)				/* NONE */
	{
		eTerminatorStatus = TERMINATOR_NONE;
	}
	else if(ST_TERM == (int)TERMINATOR_TAB)			/* TAB(OD OA) */
	{
		eTerminatorStatus = TERMINATOR_TAB;
	}
	else if(ST_TERM == (int)TERMINATOR_ENTER)		/* ENTER(OD) */
	{
		eTerminatorStatus = TERMINATOR_ENTER;
	}
	else if(ST_TERM == (int)TERMINATOR_SPACE)		/* SPACE(20) */
	{
		eTerminatorStatus = TERMINATOR_SPACE;
	}
	else if(ST_TERM == (int)TERMINATOR_RETURN)		/* RETURN(OA) */
	{
		eTerminatorStatus = TERMINATOR_RETURN;
	}

	/* 大小写 */
	if (ST_DTTYPE == (int)CASEANDCASE_RAWDATA)					/* 原数据 */
	{
		eCaseAndCaseStatus = CASEANDCASE_RAWDATA;
	}
	else if(ST_DTTYPE == (int)CASEANDCASE_UPPERCASE)				/* 强制大写 */
	{
		eCaseAndCaseStatus = CASEANDCASE_UPPERCASE;
	}
	else if(ST_DTTYPE == (int)CASEANDCASE_LOWERCASE)				/* 强制小写 */
	{
		eCaseAndCaseStatus = CASEANDCASE_LOWERCASE;
	}
	else if(ST_DTTYPE == (int)CASEANDCASE_UPPERLOWEREXCHANGE)	/* 大小写互换 */
	{
		eCaseAndCaseStatus = CASEANDCASE_UPPERLOWEREXCHANGE;
	}

	/* 扫描方式 */
	if (ST_REMOD == (int)SCANMODE_TEST)			/* 测试模式 */
	{
		eScanModeStatus = SCANMODE_TEST;
	}
	else if(ST_REMOD == (int)SCANMODE_MANUAL)	/* 手动扫描 */
	{
		eScanModeStatus = SCANMODE_MANUAL;
	}
	else if(ST_REMOD == (int)SCANMODE_LIGHT)		/* 连续常亮扫描 */
	{
		eScanModeStatus = SCANMODE_LIGHT;
	}
	else if(ST_REMOD == (int)SCANMODE_TWINKLE)	/* 连续闪烁扫描 */
	{
		eScanModeStatus = SCANMODE_TWINKLE;
	}

	/* 冗余裁决 */
	if (ST_REDUN == (int)REDUNDADJUST_ONE)		/* 一次 */
	{
		eRedundAdjustStatus = REDUNDADJUST_ONE;
	}
	else if(ST_REDUN == (int)REDUNDADJUST_TWO)	/* 二次 */
	{
		eRedundAdjustStatus = REDUNDADJUST_TWO;
	}
	else if(ST_REDUN == (int)REDUNDADJUST_THREE)	/* 三次 */
	{
		eRedundAdjustStatus = REDUNDADJUST_THREE;
	}
	else if(ST_REDUN == (int)REDUNDADJUST_FOUR)	/* 四次 */
	{
		eRedundAdjustStatus = REDUNDADJUST_FOUR;
	}

	/* 音长 */
	if (ST_BPLONG == (int)SOUNDLENGTH_SHORT)			/* 短音 */
	{
		eSoundLengthStatus = SOUNDLENGTH_SHORT;
	}
	else if(ST_BPLONG == (int)SOUNDLENGTH_MIDDLE)	/* 中音 */
	{
		eSoundLengthStatus = SOUNDLENGTH_MIDDLE;
	}
	else if(ST_BPLONG == (int)SOUNDLENGTH_LONG)		/* 长音 */
	{
		eSoundLengthStatus = SOUNDLENGTH_LONG;
	}
	else if(ST_BPLONG == (int)SOUNDLENGTH_USERDEFINE)	/* 自定义 */
	{
		eSoundLengthStatus = SOUNDLENGTH_USERDEFINE;
	}

	/* 音频 */
	if (ST_FREQ == (int)AUDIOFREQUENCY_LOW)				/* 低频 */
	{
		eAudioFrequencyStatus = AUDIOFREQUENCY_LOW;
	}
	else if(ST_FREQ == (int)AUDIOFREQUENCY_INTERMEDIATE)	/* 中频 */
	{
		eAudioFrequencyStatus = AUDIOFREQUENCY_INTERMEDIATE;
	}
	else if(ST_FREQ == (int)AUDIOFREQUENCY_HIGH)			/* 高频 */
	{
		eAudioFrequencyStatus = AUDIOFREQUENCY_HIGH;
	}
	else if(ST_FREQ == (int)AUDIOFREQUENCY_USERDEFINE)	/* 自定义 */
	{
		eAudioFrequencyStatus = AUDIOFREQUENCY_USERDEFINE;
	}

	/* 音量 */
	if (ST_SOUNDLEV == (int)VOLUME_WHISPERING)			/* 小声 */
	{
		eVolumeStatus = VOLUME_WHISPERING;
	}
	else if(ST_SOUNDLEV == (int)VOLUME_MEDIUMSOUND)		/* 中声 */
	{
		eVolumeStatus = VOLUME_MEDIUMSOUND;
	}
	else if(ST_SOUNDLEV == (int)VOLUME_LOUD)				/* 大声 */		
	{
		eVolumeStatus = VOLUME_LOUD;
	}
	else if(ST_SOUNDLEV == (int)VOLUME_USERDEFINE)		/* 自定义 */
	{
		eVolumeStatus = VOLUME_USERDEFINE;
	}

	/* KB时钟 */
	if (ST_CLK == (int)KBCLOCK_10K)					/* 10K */
	{
		eKBclockStatus = KBCLOCK_10K;
	}
	else if(ST_CLK == (int)KBCLOCK_20K)				/* 20K */
	{
		eKBclockStatus = KBCLOCK_20K;
	}
	else if(ST_CLK == (int)KBCLOCK_30K)				/* 30K */
	{
		eKBclockStatus = KBCLOCK_30K;
	}

	/* 数据截取 */
	if (ST_DRER == (int)DATAINTERCEPTION_NO)			/* 不截取 */
	{
		eDataInterceptionStatus = DATAINTERCEPTION_NO;
	}
	else if(ST_DRER == (int)DATAINTERCEPTION_LEFT)	/* 从左 */
	{
		eDataInterceptionStatus = DATAINTERCEPTION_LEFT;
	}
	else if(ST_DRER == (int)DATAINTERCEPTION_RIGHT)	/* 从右 */
	{
		eDataInterceptionStatus = DATAINTERCEPTION_RIGHT;
	}

	/* 一维反色 */
	if (ST_RESERV1D == (int)ONEREADONLYBLACKCODE)		/* 1d只读黑色码 */
	{
		eOneReverseColorStatus = ONEREADONLYBLACKCODE;
	}
	else if(ST_RESERV1D == (int)ONEREADONLYREVERSECODE)	/* 1d只读反色码 */
	{
		eOneReverseColorStatus = ONEREADONLYREVERSECODE;
	}
	else if(ST_RESERV1D == (int)ONEBOTHREAD)				/* 1d正反都读 */
	{
		eOneReverseColorStatus = ONEBOTHREAD;
	}

	/* 二维反色 */
	if (ST_RESERV2D == (int)TWOREADONLYBLACKCODE)		/* 2d只读黑色码 */
	{
		eTwoReverseColorStatus = TWOREADONLYBLACKCODE;
	}
	else if(ST_RESERV2D == (int)TWOREADONLYREVERSECODE)	/* 2d只读反色码 */
	{
		eTwoReverseColorStatus = TWOREADONLYREVERSECODE;
	}
	else if(ST_RESERV2D == (int)TWOBOTHREAD)				/* 2d正反都读 */
	{
		eTwoReverseColorStatus = TWOBOTHREAD;
	}

	/* 一维多读 */
	if (ST_APPEND1D == (int)ONEREAdMORE_CLOSE)			/* 一维多个读取关闭 */
	{
		eOneReadMoreStatus = ONEREAdMORE_CLOSE;
	}
	else if(ST_APPEND1D == (int)ONEREAdMORE_OPEN)		/* 一维多个读取打开 */
	{
		eOneReadMoreStatus = ONEREAdMORE_OPEN;
	}

	/* 二维多读 */
	if (ST_APPEND2D == (int)TWOREADMORE_CLOSE)			/* 二维多个读取关闭 */
	{
		eTwoReadMoreStatus = TWOREADMORE_CLOSE;
	}
	else if(ST_APPEND2D == (int)TWOREADMORE_OPEN)		/* 二维多个读取打开 */
	{
		eTwoReadMoreStatus = TWOREADMORE_OPEN;
	}

	/* 解码休眠 */
	if (ST_DCSLEEP == (int)DECODESLEEP_NON)				/* 解码不休眠 */
	{
		eDecodeSleepStatus = DECODESLEEP_NON;
	}
	else if(ST_DCSLEEP == (int)DECODESLEEP_1S)			/* 1s */
	{
		eDecodeSleepStatus = DECODESLEEP_1S;
	}
	else if(ST_DCSLEEP == (int)DECODESLEEP_1MIN)			/* 1min */
	{
		eDecodeSleepStatus = DECODESLEEP_1MIN;
	}
	else if(ST_DCSLEEP == (int)DECODESLEEP_10MIN)		/* 10min */
	{
		eDecodeSleepStatus = DECODESLEEP_10MIN;
	}
	else if(ST_DCSLEEP == (int)DECODESLEEP_30MIN)		/* 30min */
	{
		eDecodeSleepStatus = DECODESLEEP_30MIN;
	}
	else if(ST_DCSLEEP == (int)DECODESLEEP_USER)			/* user */
	{
		eDecodeSleepStatus = DECODESLEEP_USER;
	}

	/* 引擎休眠 */
	if (ST_ENSLEEP == (int)ENGINESLEEP_NON)				/* 引擎不休眠 */
	{
		eEngineSleepStatus = ENGINESLEEP_NON;
	}
	else if(ST_ENSLEEP == (int)ENGINESLEEP_IMMEDIATELY)	/* 引擎立即休眠 */
	{
		eEngineSleepStatus = ENGINESLEEP_IMMEDIATELY;
	}
	else if(ST_ENSLEEP == (int)ENGINESLEEP_1MIN)			/* 1min */
	{
		eEngineSleepStatus = ENGINESLEEP_1MIN;
	}
	else if(ST_ENSLEEP == (int)ENGINESLEEP_10MIN)		/* 10min */
	{
		eEngineSleepStatus = ENGINESLEEP_10MIN;
	}
	else if(ST_ENSLEEP == (int)ENGINESLEEP_30MIN)		/* 30min */
	{
		eEngineSleepStatus = ENGINESLEEP_30MIN;
	}
	else if(ST_ENSLEEP == (int)ENGINESLEEP_USER)			/* user */
	{
		eEngineSleepStatus = ENGINESLEEP_USER;
	}
	
	/* 读取延时 */
	if (ST_LAYSERDELAY == (int)READDELAY_NOT)			/* 激光/读取延时关 */
	{
		eReadDelayStatus = READDELAY_NOT;
	}
	else if(ST_LAYSERDELAY == (int)READDELAY_300MS)		/* 300ms */
	{
		eReadDelayStatus = READDELAY_300MS;
	}
	else if(ST_LAYSERDELAY == (int)READDELAY_600MS)		/* 600ms */
	{
		eReadDelayStatus = READDELAY_600MS;
	}
	else if(ST_LAYSERDELAY == (int)READDELAY_900MS)		/* 900ms */
	{
		eReadDelayStatus = READDELAY_900MS;
	}
	else if(ST_LAYSERDELAY == (int)READDELAY_USER)		/* user */
	{
		eReadDelayStatus = READDELAY_USER;
	}

	/* 附加码读取延时 */
	if (ST_ADD_DELAY == (int)ADDITIONCODERDDELAY_CLOSE)		/* 附加码延时关 */
	{
		eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_CLOSE;
	}
	else if(ST_ADD_DELAY == (int)ADDITIONCODERDDELAY_100MS)	/* 100ms */
	{
		eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_100MS;
	}
	else if(ST_ADD_DELAY == (int)ADDITIONCODERDDELAY_200MS)	/* 200ms */
	{
		eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_200MS;
	}
	else if(ST_ADD_DELAY == (int)ADDITIONCODERDDELAY_400MS)	/* 400ms */
	{
		eAdditionCodeRdDelayStatus = ADDITIONCODERDDELAY_400MS;
	}

	/* AUX */
	if (ST_AUX_MOD == (int)AXUSTATUS_CLOSE)					/* AUX关闭 */
	{
		eAuxStatusStatus = AXUSTATUS_CLOSE;
	}
	else if(ST_AUX_MOD == (int)AXUSTATUS_EMBEDDEDDATA)		/* 嵌入数据 */
	{
		eAuxStatusStatus = AXUSTATUS_EMBEDDEDDATA;
	}
	else if(ST_AUX_MOD == (int)AXUSTATUS_TRANS)				/* 透传 */
	{
		eAuxStatusStatus = AXUSTATUS_TRANS;
	}
	
	
}

int EnterSetMode(char *pData)
{
	int ret = 0;

	long setcode = StrToHex(pData+1, strlen(pData+1));

	SetCode(setcode);

	return ret;
}
