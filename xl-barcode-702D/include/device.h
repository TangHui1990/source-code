#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <control.h>

extern int keyboard_fill_report(char report[8], char buf[2]);
extern int Hid_Output(const char *str, int cmd_len);
extern int Hid_Init(void);
extern int Hid_Exit(void);
extern int EnterSetMode(const char *pData);
extern int MoveDetect(unsigned char* p,unsigned char* q,int width,int height);
extern int MoveDetect(unsigned char* p,unsigned char* q,int width,int height);
extern int readFrame(void);
extern int uart_read(char *cbuf, int ilen);
extern int XL_IsSetCodeInput(const char *pInputData, int iLength);
extern int uart_write(char *cbuf, int iLen);
extern int Decode_DataProcessing(char *pSrcDat, int iLen);
extern int uart_init(void);
extern void InitDebufInfo(void);
extern int uart_init(void);
extern int camera_init(void);
extern int XL_MT9V022_QrCodeDealWith(void);
extern void camera_exit(void);

extern NakHandshake NAKStatus;  					/* NAK握手 */
extern HardwareContolFlow iHdflowStatus;			/* 硬件控制流 */
extern XonHandshake iXONStatus;					/* XON握手 */
extern ComTriggerSwitch iComTriggerSwitchStatus;	/* COM触发开关 */
extern SetContolRight iSetCtrlRightStatus;  		/* 设置控制权 */
extern SerialportSetVoice eSerialPortStatus; 		/* 串口设置提示音 */
extern FunctionalKeyboard eFuncKeyStatus;			/* 功能键盘 */
extern Keypad eFunckeypadStatus;					/* 小键盘 */
extern CaseIgnored eCaseIgnoreStatus;				/* 大小写忽略 */
extern AIMcode eAimCodeSetStatus;					/* AIM码 */
extern CodeID eCodeIdSetStatus;					/* CODE ID */
extern Febraban eFebrabanFuncSetStatus;			/* Febraban */
extern BootSound eBootSoundSetStatus;				/* 开机音 */
extern DataInversion eDecodeDataInversionStatus;	/* 数据倒置 */
extern LengthPrefix eLengthPrefixSetStatus;		/* 长度前缀 */
extern BellTransmition eBellTransmitStatus;		/* 响铃传输 */
extern BELfunc eBleFuncSetStatus;					/* BEL功能 */
extern SleepTipVioce eSleepTipVioceStatus;			/* 休眠提示音 */
extern Tropitic eTropiticStatus;					/* tropitic39 */
extern AutomaticInduction eAutomaticInductionStatus;/* 自动感应 */
extern EANstrengthen eEanStrenthenSetStatus;		/* EAN13加强 */
extern ISBTstatus eIsbtStatusSetStatus;			/* ISBT */
extern LedTips eLedTipsStatus;						/* LED提示 */
extern ReadSameCode eRedSameCodeStatus;			/* 相同条码读取 */
extern ReadPresupposition eReadPresuppositionStatus;/* 读取预设 */
extern FillinLight eFillinLightSetStatus;			/* 补光灯 */
extern MarkLamp eMarkLampStatus;					/* 标示灯 */
extern CommandEcho eCommandEchoStatus;				/* 命令回应 */
extern Interface eInterfaceStatus;					/* 接口 */
extern Language eLanguageStatus;					/* 语言 */
extern BaudRate eBaudRateStatus;					/* 波特率 */
extern DataBit eDataBitStatus;						/* 数据位 */
extern CheckoutBit eCheckoutBitStatus;				/* 校验位 */
extern StopBit eStopBitStatus;						/* 停止位 */
extern Terminator eTerminatorStatus;				/* 结束符 */
extern CaseAndCase eCaseAndCaseStatus;				/* 大小写 */
extern ScanMode eScanModeStatus;					/* 扫描方式 */
extern RedundAdjust eRedundAdjustStatus;			/* 冗余裁决 */
extern SoundLength eSoundLengthStatus;				/* 音长 */
extern AudioFrequency eAudioFrequencyStatus;		/* 音频 */
extern Volume eVolumeStatus;						/* 音量 */
extern KBclock eKBclockStatus;						/* KB时钟 */
extern DataInterception eDataInterceptionStatus;	/* 数据截取 */
extern OneReverseColor eOneReverseColorStatus;		/* 一维反色 */
extern TwoReverseColor eTwoReverseColorStatus;		/* 二维反色 */
extern OneReadMore eOneReadMoreStatus;				/* 一维多读 */
extern TwoReadMore eTwoReadMoreStatus;				/* 二维多读 */
extern DecodeSleep eDecodeSleepStatus;				/* 解码休眠 */
extern EngineSleep eEngineSleepStatus;				/* 引擎休眠 */
extern ReadDelay eReadDelayStatus;					/* 读取延时 */
extern AdditionCodeRdDelay eAdditionCodeRdDelayStatus;/* 附加码读取延时 */
extern AuxStatus eAuxStatusStatus;					/* AUX */

extern CapsLockMode eCapsLockStatus;

extern int hid_fd;
extern int camera_fd;
extern int BeepFd;
extern int buttons_fd;
extern unsigned char SetModeFlag;
extern unsigned int iVoiceCtrl;

struct Buffer {
	void *start;
	size_t	length;
};

extern struct Buffer *buffers;

#define CONFIG_SOC_CAMERA_GC0308
//#define CONFIG_SOC_CAMERA_AR0144
//#define CONFIG_SOC_CAMERA_MT9V022

extern CapsLockMode eCapsLockStatus;


#ifdef CONFIG_SOC_CAMERA_MT9V022
#define PREVIEW_WIDTH	752
#define PREVIEW_HEIGHT	480
#endif

#ifdef CONFIG_SOC_CAMERA_GC0308
#define PREVIEW_WIDTH	640
#define PREVIEW_HEIGHT	480
#endif

#ifdef CONFIG_SOC_CAMERA_AR0144
#define PREVIEW_WIDTH	960
#define PREVIEW_HEIGHT	640
#endif


#endif






