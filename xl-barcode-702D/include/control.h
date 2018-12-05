#ifndef __CONTROL_H_
#define __CONTROL_H_

#define SHORTTONE_SOUND    		0x1<<0		/* 短音 */
#define MIDDLETONE_SOUND   		0x1<<1		/* 中音 */
#define LONGTONE_SOUND	   		0x1<<2		/* 长音 */

#define LOWFREQUENCY_SOUND		0x1<<3		/* 低频 */
#define MIDDLEFREQUENCY_SOUND 	0x1<<4		/* 中频 */
#define HIGHFREQUENCY_SOUND	  	0x1<<5		/* 高频 */

#define LOWVOICE_SOUND			0x1<<6		/* 小声 */
#define MIDDLEVOICE_SOUND		0x1<<7		/* 中声 */
#define HIGHVOICE_SOUND			0x1<<8		/* 大声 */

#define JZ_IOC_MAGIC 'k'
#define DECODESUCCESS_SOUND  _IOW(JZ_IOC_MAGIC, 1, int)		/* 解码成功蜂鸣器声音 */
#define BOOTPROMPT_SOUND  _IOW(JZ_IOC_MAGIC, 2, int)		/* 开机提示音 */
#define COMSETPROMPT_SOUND  _IOW(JZ_IOC_MAGIC, 3, int)		/* 串口设置提示音 */
#define DORMANTPROMPT_SOUND  _IOW(JZ_IOC_MAGIC, 4, int)		/* 休眠提示音 */
#define BELLTRANSMITION_SOUND  _IOW(JZ_IOC_MAGIC, 5, int)	/* 响铃传输 */

#define JZ_PWM_OFF  _IO(JZ_IOC_MAGIC, 6)
#define JZ_LED_ON  _IO(JZ_IOC_MAGIC, 7)
#define JZ_LED_OFF  _IO(JZ_IOC_MAGIC, 8)
#define JZ_LED_DELAY_OFF  _IO(JZ_IOC_MAGIC, 9)
#define JZ_LED_TIP_ON  _IO(JZ_IOC_MAGIC, 10)     	/* LED提示灯开启 */
#define JZ_LED_TIP_OFF  _IO(JZ_IOC_MAGIC, 11)		/* LED提示灯关闭 */

#define NAK_MAGIC 'N'
/*定义命令*/ 
#define NAK_TIMER_5MS   _IO(NAK_MAGIC, 1)
#define NAK_TIMER_10MS  _IO(NAK_MAGIC, 2)
#define NAK_TIMER_20MS  _IO(NAK_MAGIC, 3)
#define NAK_TIMER_30MS  _IO(NAK_MAGIC, 4)
#define NAK_TIMER_50MS  _IO(NAK_MAGIC, 5)
#define NAK_TIMER_80MS  _IO(NAK_MAGIC, 6)
#define NAK_TIMER_USER  _IO(NAK_MAGIC, 7)

/* 输出方式选择 */
typedef enum 
{
	HID_OUTPUT,                 /* HID输出 */
	VITUAL_SERIAL_OUTPUT,       /* 虚拟串口输出 */
	PHYSICAL_SERIAL_OUTPUT,     /* 物理串口输出 */
}OutputMode;

/* CapsLock是否被按下 */
typedef enum
{
	CAPSLOCK_PRESS,				/* CapsLock按下 */
	CAPSLOCK_NOPRESS,			/* CapsLock没有按下 */
}CapsLockMode;

/* NAK握手 */
typedef enum
{
	HANDSHAKE_CLOSE, 	/* NAK握手关闭 */
	HANDSHAKE_OPEN,  	/* NAK握手开启 */
}NakHandshake;

/* 硬件控制流 */
typedef enum
{
	HARDWARECONTROLFLOW_CLOSE, 	/* 控制流关闭 */
	HARDWARECONTROLFLOW_OPEN,	/* 控制流开启 */
}HardwareContolFlow;

/* XON握手 */
typedef enum
{
	XONHANDSHAKE_OPEN, 			/* XON握手开启 */
	XONHANDSHAKE_CLOSE,			/* XON握手关闭 */
}XonHandshake;

/* COM触发开关 */
typedef enum
{
	COM_READ_WRITE_ALLOWED,   	/* COM充许触发读写 */
	TRIGGER_UNLIMITED,			/* 不充许 */
}ComTriggerSwitch;

/* 设置控制权 */
typedef enum
{	
	CONTROL_UNLIMITED,			/* 无限制 */
	BARCODE_SET_PROHIBITED,    	/* 禁止条码设置 */
	COM_SET_PROHIBITED,			/* 禁止COM设置 */
}SetContolRight;

/* 串口设置提示音 */
typedef enum
{
	SERIALPORTSETVOICE_CLOSE,   /* 串口设置提示音关闭 */
	SERIALPORTSETVOICE_OPEN,	/* 串口设置提示音开启 */
}SerialportSetVoice;

/* 功能键盘 */
typedef enum
{
	FUNCTIONALKEYBOARD_CLOSE,	/* 功能键盘关闭 */
	FUNCTIONALKEYBOARD_OPNE,	/* 功能键盘开启 */
}FunctionalKeyboard;		

/* 小键盘 */
typedef enum
{
	KEYPAD_CLOSE,				/* 小键盘关闭 */
	KEYPAD_OPEN,				/* 小键盘开启 */
}Keypad;

/* 大小写忽略 */
typedef enum
{
	CASEIGNORED_CLOSE,			/* 大小写忽略关闭 */
	CASEIGNORED_OPEN,			/* 大小写忽略开启 */
}CaseIgnored;

/* AIM码 */
typedef enum
{
	AIMCODE_CLOSE,				/* AIM码关闭 */
	AIMCODE_OPEN,				/* AIM码开启 */
}AIMcode;

/* CODE ID */
typedef enum
{
	CODEID_CLOSE,				/* CODE ID 关闭 */
	CODEID_OPEN,				/* CODE ID 开启 */
}CodeID;

/* Febraban */
typedef enum
{
	FEBRABAN_CLOSE,				/* Febraban 关闭 */
	FEBRABAN_OPEN,				/* Febraban 开启 */
}Febraban;

/* 开机音 */
typedef enum
{
	BOOTSOUND_CLOSE,			/* 开机音关闭 */
	BOOTSOUND_OPEN,				/* 开机音开启 */
}BootSound;

/* 数据倒置 */
typedef enum
{
	DATAINVERSION_CLOSE,		/* 数据倒置关闭 */
	DATAINVERSION_OPEN,			/* 数据倒置开启 */
}DataInversion;

/* 长度前缀 */
typedef enum
{
	LENGTHPREFIX_CLOSE,			/* 长度前缀关闭 */
	LENGTHPROFIX_OPEN,			/* 长度前缀开启 */
}LengthPrefix;

/* 响铃传输 */
typedef enum
{
	BELLTRANSMITION_CLOSE,		/* 响铃传输关闭 */
	BELLTRANSMITION_OPEN,		/* 响铃传输开启 */
}BellTransmition;

/* BEL功能 */
typedef enum
{
	BELFUNC_CLOSE,				/* BEL功能关闭 */
	BELFUNC_OPEN,				/* BEL功能开启 */
}BELfunc;

/* 休眠提示音 */
typedef enum
{
	SLEEPTIPVIOCE_CLOSE,		/* 休眠提示音关闭 */
	SLEEPTIPVIOCE_OPEN,			/* 休眠提示音开启 */
}SleepTipVioce;

/* tropitic39 */
typedef enum
{
	TROPITIC_CLOSE,				/* tropitic39关闭 */
	TROPITIC_OPEN,				/* tropitic39开启 */
}Tropitic;

/* 自动感应 */
typedef enum
{
	AUTOMATICINDUCTION_CLOSE,	/* 自动感应关闭 */
	AUTOMATICINDUCTION_OPEN,	/* 自动感应开启 */
}AutomaticInduction;

/* EAN13 加强 */
typedef enum
{
	EANSTRENGTHEN_CLOSE,		/* EAN13加强关闭 */
	EANSTRENGTHEN_OPEN,			/* EAN13加强开启 */
}EANstrengthen;

/* ISBT */
typedef enum
{
	ISBT_CLOSE,					/* ISBT关闭 */
	ISBT_OPEN,					/* ISBT开启 */
}ISBTstatus;

/* LED提示 */
typedef enum
{
	LEDTIPS_CLOSE,				/* LED提示关闭 */
	LEDTIPS_OPEN,				/* LED提示开启 */
}LedTips;

/* 相同条码读取 */
typedef enum
{
	READSAMECODE_CLOSE,			/* 相同条码读取关闭 */
	READSAMECODE_OPEN,			/* 相同条码读取开启 */
}ReadSameCode;

/* 读取预设 */
typedef enum
{
	READpresupposition_AUTO, 	/* 读取预设自动 */
	READpresupposition_MONITOR,	/* 读取预设显示器 */
	READpresupposition_PAPER,	/* 读取预设纸张 */
}ReadPresupposition;

/* 补光灯 */
typedef enum
{
	FILLINLIGHT_CLOSE,				/* 补光灯关闭 */
	FILLINLIGHT_LOWBRIGHTNESS,		/* 补光灯低亮度 */
	FILLINLIGHT_MEDIUMBRIGHTNESS,	/* 补光灯中亮度 */
	FILLINLIGHT_HIGHBRIGHTNESS,		/* 补光灯高亮度 */
}FillinLight;

/* 标示灯 */
typedef enum
{
	MARKLAMP_CLOSE, 				/* 标示灯关 */
	MARKLAMP_OPEN,					/* 标示灯开 */
}MarkLamp;

/* 命令回应 */
typedef enum
{
	SERIAL_COMMAND_NO_RETURN,		/* 串口命令无返回 */
	SERIAL_COMMAND_RETURN,			/* 串口命令有返回 */
}CommandEcho;

/* 接口 */
typedef enum
{
	INTERFACE_AUTO,					/* 自动 */
	INTERFACE_KB,					/* KB */
	INTERFACE_HID,					/* HID */
	INTERFACE_VCP,					/* VCP */
}Interface;

/* 语言 */
typedef enum
{
	LANGUAGE_COMMON,  				/* 通用 */
	LANGUAGE_USA,					/* 美国 */
	LANGUAGE_FRANCE,				/* 法国 */
	LANGUAGE_GERMANY,				/* 德国 */
	LANGUAGE_BRITAIN,				/* 英国 */
	LANGUAGE_TURKEY,				/* 土耳其 */
	LANGUAGE_DENMARK,				/* 丹麦 */
	LANGUAGE_JAPAN,					/* 日本 */
	LANGUAGE_SPAIN,					/* 西班牙 */
	LANGUAGE_ITALY,					/* 意大利 */
	LANGUAGE_JACK,					/* 杰克 */
	LANGUAGE_SNOVUCK,				/* 斯诺伐克 */
}Language;

/* 波特率 */
typedef enum
{
	BAUDRATE_1200N,					/* 波特率1200 */
	BAUDRATE_2400N,					/* 波特率2400 */
	BAUDRATE_4800N,					/* 波特率4800 */
	BAUDRATE_9600N,					/* 波特率9600 */
	BAUDRATE_14400N,				/* 波特率14400 */
	BAUDRATE_19200N,				/* 波特率19200 */
	BAUDRATE_28800N,				/* 波特率28800 */
	BAUDRATE_38400N,				/* 波特率38400 */
	BAUDRATE_57600N,				/* 波特率57600 */
	BAUDRATE_115200N,				/* 波特率115200 */
}BaudRate;

/* 数据位 */
typedef enum
{
	DATABIT_7BIT,					/* 7位 */
	DATABIT_8BIT,					/* 8位 */
}DataBit;

/* 校验位 */
typedef enum
{
	CHECKOUTBIT_NONE,				/* 无校验 */
	CHECKOUTBIT_ODD,				/* 奇校验 */
	CHECKOUTBIT_PARITY,				/* 偶校验 */
	CHECKOUTBIT_EMPTY,				/* 空校验 */
	CHECKOUTBIT_LABLE,				/* 标记校验 */
}CheckoutBit;

/* 停止位 */
typedef enum
{
	STOPBIT_1BIT,					/* 一位 */
	STOPBIT_2BIT,					/* 二位 */
}StopBit;

/* 结束符 */
typedef enum
{
	TERMINATOR_NONE,				/* NONE */
	TERMINATOR_TAB,					/* TAB(OD OA) */
	TERMINATOR_ENTER,				/* ENTER(OD) */
	TERMINATOR_SPACE,				/* SPACE(20) */
	TERMINATOR_RETURN,				/* RETURN(OA) */
}Terminator;

/* 大小写 */
typedef enum
{
	CASEANDCASE_RAWDATA,			/* 原数据 */
	CASEANDCASE_UPPERCASE,			/* 强制大写 */
	CASEANDCASE_LOWERCASE,			/* 强制小写 */
	CASEANDCASE_UPPERLOWEREXCHANGE,	/* 大小写互换 */
}CaseAndCase;

/* 扫描方式 */
typedef enum
{
	SCANMODE_TEST,         	 		/* 测试模式 */
	SCANMODE_MANUAL,				/* 手动扫描 */
	SCANMODE_LIGHT,					/* 连续常亮扫描 */
	SCANMODE_TWINKLE,				/* 连续闪烁扫描 */
}ScanMode;

/* 冗余裁决 */
typedef enum
{
	REDUNDADJUST_ONE,				/* 一次 */
	REDUNDADJUST_TWO,				/* 二次 */
	REDUNDADJUST_THREE,				/* 三次 */
	REDUNDADJUST_FOUR,				/* 四次 */
}RedundAdjust;

/* 音长 */
typedef enum
{
	SOUNDLENGTH_SHORT,				/* 短音 */
	SOUNDLENGTH_MIDDLE,				/* 中音 */
	SOUNDLENGTH_LONG,				/* 长音 */
	SOUNDLENGTH_USERDEFINE,			/* 自定义 */
}SoundLength;

/* 音频 */
typedef enum
{
	AUDIOFREQUENCY_LOW,				/* 低频 */
	AUDIOFREQUENCY_INTERMEDIATE,	/* 中频 */
	AUDIOFREQUENCY_HIGH,			/* 高频 */
	AUDIOFREQUENCY_USERDEFINE,		/* 自定义 */
}AudioFrequency;

/* 音量 */
typedef enum
{
	VOLUME_WHISPERING,				/* 小声 */
	VOLUME_MEDIUMSOUND,				/* 中声 */
	VOLUME_LOUD,					/* 大声 */
	VOLUME_USERDEFINE,				/* 自定义 */
}Volume;

/* KB时钟 */
typedef enum
{
	KBCLOCK_10K,					/* 10K */
	KBCLOCK_20K,					/* 20K */
	KBCLOCK_30K,					/* 30K */
}KBclock;

/* 数据截取 */
typedef enum
{
	DATAINTERCEPTION_NO,			/* 不截取 */
	DATAINTERCEPTION_LEFT,			/* 从左 */
	DATAINTERCEPTION_RIGHT,			/* 从右 */
}DataInterception;

/* 一维反色 */
typedef enum
{
	ONEREADONLYBLACKCODE,			/* 1d只读黑色码 */
	ONEREADONLYREVERSECODE,			/* 1d只读反色码 */
	ONEBOTHREAD,					/* 1d正反都读 */
}OneReverseColor;

/* 二维反色 */
typedef enum
{	
	TWOREADONLYBLACKCODE,			/* 2d只读黑色码 */
	TWOREADONLYREVERSECODE, 		/* 2d只读反色码 */
	TWOBOTHREAD,					/* 2d正反都读 */
}TwoReverseColor;

/* 一维多读 */
typedef enum
{
	ONEREAdMORE_CLOSE,				/* 一维多个读取关闭 */
	ONEREAdMORE_OPEN,				/* 一维多个读取打开 */
}OneReadMore;

/* 二维多读 */
typedef enum
{
	TWOREADMORE_CLOSE,				/* 二维多个读取关闭 */
	TWOREADMORE_OPEN,				/* 二维多个读取打开 */
}TwoReadMore;

/* 解码休眠 */
typedef enum
{
	DECODESLEEP_NON,				/* 解码不休眠 */
	DECODESLEEP_1S,					/* 1s */
	DECODESLEEP_1MIN,				/* 1min */
	DECODESLEEP_10MIN,				/* 10min */
	DECODESLEEP_30MIN,				/* 30min */
	DECODESLEEP_USER,				/* user */
}DecodeSleep;

/* 引擎休眠 */
typedef enum
{
	ENGINESLEEP_NON,				/* 引擎不休眠 */
	ENGINESLEEP_IMMEDIATELY,		/* 引擎立即休眠 */
	ENGINESLEEP_1MIN,				/* 1min */
	ENGINESLEEP_10MIN,				/* 10min */
	ENGINESLEEP_30MIN,				/* 30min */
	ENGINESLEEP_USER,				/* user */
}EngineSleep;

/* 读取延时 */
typedef enum
{
	READDELAY_NOT,					/* 激光/读取延时关 */
	READDELAY_300MS,				/* 300ms */
	READDELAY_600MS,				/* 600ms */
	READDELAY_900MS,				/* 900ms */
	READDELAY_USER,					/* user */
}ReadDelay;

/* 附加码读取延时 */
typedef enum
{
	ADDITIONCODERDDELAY_CLOSE,		/* 附加码延时关 */
	ADDITIONCODERDDELAY_100MS,		/* 100ms */
	ADDITIONCODERDDELAY_200MS,		/* 200ms */
	ADDITIONCODERDDELAY_400MS,		/* 400ms */
}AdditionCodeRdDelay;

/* AUX */
typedef enum
{
	AXUSTATUS_CLOSE,				/* AUX关闭 */
	AXUSTATUS_EMBEDDEDDATA,			/* 嵌入数据 */
	AXUSTATUS_TRANS,				/* 透传 */
}AuxStatus;

#endif




















