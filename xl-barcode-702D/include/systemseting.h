#ifndef __SETTING_H_
#define __SETTING_H_

extern unsigned int SETTAB[];
extern const unsigned char DEFAULTROM[];

#define basic 0		 //100/128

//标准25
#define READ_EN_I25			SETTAB[basic+0]	//OK			
#define NEED_CK_I25			SETTAB[basic+1]	//OK
#define TRANS_CK_I25		SETTAB[basic+2]	//OK

//工业25					
#define READ_EN_S25			SETTAB[basic+3]	//OK
#define NEED_CK_S25			SETTAB[basic+4]	//OK
#define TRANS_CK_S25		SETTAB[basic+5]	//OK
										
//邮政25				
#define READ_EN_P25			SETTAB[basic+6]	//OK
#define NEED_CK_P25			SETTAB[basic+7]	//OK
#define TRANS_CK_P25		SETTAB[basic+8]	//OK

//交叉25
#define READ_EN_ITF25		SETTAB[basic+9]		//OK
#define NEED_CK_ITF25		SETTAB[basic+10]	//OK
#define TRANS_CK_ITF25		SETTAB[basic+11]	//OK
#define TRANS_HEAD0_ITF25 	SETTAB[basic+12]	//OK

//矩阵25
#define READ_EN_M25			SETTAB[basic+13]	//OK
#define NEED_CK_M25			SETTAB[basic+14]	//OK
#define TRANS_CK_M25		SETTAB[basic+15]	//OK

//CODABAR					
#define READ_EN_CB			SETTAB[basic+16]	//OK
#define NEED_CK_CB			SETTAB[basic+17]	//OK
#define TRANS_START_EN_CB	SETTAB[basic+18]	//OK
#define S_E_ABCD_CB			SETTAB[basic+19]	//OK
#define TRANS_CK_CB			SETTAB[basic+20]	//OK


//MSI
#define READ_EN_MSI			SETTAB[basic+21]	//OK
#define NEED_CK1_MSI		SETTAB[basic+22]	//OK			
#define NEED_CK2_MSI		SETTAB[basic+23]	//OK
#define FIST_M10M11_MSI		SETTAB[basic+24]	//OK
#define SECD_M10M11_MSI		SETTAB[basic+25]	//OK
#define TRANS_CK_MSI		SETTAB[basic+26]	//OK

//UK
#define READ_EN_UK			SETTAB[basic+27]	//OK
#define NEED_CK_UK			SETTAB[basic+28]	//?
#define TRANS_CK_UK			SETTAB[basic+29]	//OK


//CODE 11
#define READ_EN_C11			SETTAB[basic+30]  //OK
#define NEED_CK1_C11		SETTAB[basic+31]  //
#define NEED_CK2_C11		SETTAB[basic+32]  //
#define FIST_M11_C11		SETTAB[basic+33]  //
#define SECD_M11_C11		SETTAB[basic+34]  //
#define TRANS_CK_C11		SETTAB[basic+35]  //

//CODE 39
#define READ_EN_C39			SETTAB[basic+36]   //OK
#define NEED_CK_C39			SETTAB[basic+37]   //
#define FULL_ASCII_C39		SETTAB[basic+38]   //
#define TRANS_CK_C39		SETTAB[basic+39]   //
#define START_END39_C39		SETTAB[basic+40]   //
#define READ_C32_C39		SETTAB[basic+41]   //
#define TRANS_C32A_C39		SETTAB[basic+42]   //
#define TRANS_CK_C32		SETTAB[basic+43]	//
//#define READ_TRIOPIC_C39	SETTAB[42]
//#define START_ENDTROPIC_C39	SETTAB[43]

//CODE93
#define READ_EN_C93			SETTAB[basic+44]   //OK
#define TRANS_CK_C93		SETTAB[basic+45]   //

//ADD25
#define ADD2_ON				SETTAB[basic+46]  //OK
#define ADD5_ON				SETTAB[basic+47]  //
#define ADD_SPACE			SETTAB[basic+48]  //

//UPCA						
#define READ_EN_UPCA		SETTAB[basic+49]  //OK
#define TRANS_LEAD0_UPCA	SETTAB[basic+50]  //
#define TO_EAN13_UPCA		SETTAB[basic+51]  //
#define TRANS_CK_UPCA		SETTAB[basic+52]  //

//UPCE
#define READ_EN_UPCE		SETTAB[basic+53]  //OK
#define TRANS_LEAD0_UPCE	SETTAB[basic+54]  //
#define TO_UPCA_UPCE		SETTAB[basic+55]  //
#define TO_EAN13_UPCE		SETTAB[basic+56]  //
#define TRANS_CK_UPCE		SETTAB[basic+57]  //

//EAN13
#define READ_EN_EAN13		SETTAB[basic+58]  //OK
#define TRANS_FIRST0_EAN13	SETTAB[basic+59]  //
#define TRANS_SECOD0_EAN13	SETTAB[basic+60]  //
#define ISBN_EAN13			SETTAB[basic+61]  //
#define ISSN_EAN13			SETTAB[basic+62]  //
#define ADD_378_379_EAN13	SETTAB[basic+63]  //
#define ADD_978_977_EAN13	SETTAB[basic+64]  //
#define ADD_434_439_EAN13	SETTAB[basic+65]  //
#define ADD_419_414_EAN13	SETTAB[basic+66]  //
#define ADD_491_EAN13		SETTAB[basic+67]  //
#define ADD_978_192_EAN13	SETTAB[basic+68]  //
#define TRANS_CK_EAN13		SETTAB[basic+69]  //

//EAN8
#define READ_EN_EAN8		 SETTAB[basic+70]  //OK
#define READ_LEAD0_EAN8		 SETTAB[basic+71]  //
#define TO_UPCA_EAN8		SETTAB[basic+72]   //
#define TO_EAN13_EAN8		SETTAB[basic+73]   //
#define TRANS_CK_EAN8		SETTAB[basic+74]   //

//CODE128
#define READ_EN_C128		SETTAB[basic+75]  //OK
#define UCC_C128			SETTAB[basic+76]  //
#define TRANS_CK_C128		SETTAB[basic+77]  //

//GS1ENABLE
#define READ_EN_GS1			SETTAB[basic+78]  //OK
//RSS14								
#define READ_EN_RSS14		SETTAB[basic+79]  //OK
#define AI_RSS14			SETTAB[basic+80]	
#define TRANS_CK_RSS14		SETTAB[basic+81]

//LIMITED
#define READ_EN_LIMITED		SETTAB[basic+82]  //OK
#define AI_LIMITED			SETTAB[basic+83]
#define TRANS_CK_LIMITED	SETTAB[basic+84]
										
//EXPENDED
#define READ_EN_EXPENDED	SETTAB[basic+85]  //OK

//telepen
#define READ_EN_TELE		SETTAB[basic+86]  //OK

//pdf417
#define READ_EN_PDF	        SETTAB[basic+87]  //OK	
#define READ_EN_MPDF	    SETTAB[basic+88]  //OK

//qr
#define READ_EN_QR	        SETTAB[basic+89]  //OK	
#define READ_EN_MQR	        SETTAB[basic+90]  //OK

//aztec
#define READ_EN_AZ	        SETTAB[basic+91]  //OK

//data matrix
#define READ_EN_DM	        SETTAB[basic+92]  //OK
#define READ_RECTAN	        SETTAB[basic+93]  //OK

//sensible
#define READ_EN_HX	        SETTAB[basic+94]  //OK

//NAK握手
#define ST_NAK	        	SETTAB[basic+100]

//硬件控制流
#define ST_CTS	       		SETTAB[basic+101]

//XON握手
#define ST_XON	        	SETTAB[basic+102]

//COM触发开关
#define ST_COMM	        	SETTAB[basic+103]

//设置控制权
#define ST_COMOWN	        SETTAB[basic+104]

//串口设置提示音
#define ST_COMBEEP	        SETTAB[basic+105]

//功能键盘
#define ST_FUNKEY	        SETTAB[basic+106]

//小键盘
#define ST_NUMLOCK	        SETTAB[basic+107]

//大小写忽略
#define ST_CAPIG	        SETTAB[basic+108]

//AIM码
#define ST_AIM	        	SETTAB[basic+109]

//CODE ID
#define ST_CID	        	SETTAB[basic+110]

//Febraban
#define ST_BZCODE	        SETTAB[basic+111]

//开机音
#define ST_TIPS	        	SETTAB[basic+112]

//数据倒置
#define ST_RDBACK	        SETTAB[basic+113]

//长度前缀
#define ST_LENGTH	        SETTAB[basic+114]

//响铃传输
#define ST_BELFIRST	        SETTAB[basic+115]

//BEL功能
#define ST_BEL07	        SETTAB[basic+116]

//休眠提示音
#define ST_SLEEPBEEP	    SETTAB[basic+117]

//tropitic39
#define ST_TROPITIC39	    SETTAB[basic+118]

//自动感应
#define ST_IRON	    		SETTAB[basic+119]

//EAN13加强
#define ST_EANDEC	    	SETTAB[basic+120]

//ISBT
#define ST_ISBT	    		SETTAB[basic+121]

//LED提示
#define ST_LED	    		SETTAB[basic+122]

//相同条码读取
#define ST_SAMEREAD	        SETTAB[basic+123]

//读取预设
#define ST_MONITER_ON	    SETTAB[basic+124]

//补光灯
#define ST_LED_LUX	     	SETTAB[basic+125]

//标示灯
#define ST_TAR_ON	     	SETTAB[basic+126]

//命令回应
#define ST_COM_ACK	        SETTAB[basic+127]

//接口
#define ST_PORT	        	SETTAB[basic+130]

//语言
#define ST_LANG	        	SETTAB[basic+131]

//波特率
#define ST_BAUTR	        SETTAB[basic+132]

//数据位
#define ST_DATA	        	SETTAB[basic+133]

//校验位
#define ST_CHECK	    	SETTAB[basic+134]

//停止位
#define ST_STOP	        	SETTAB[basic+135]

//结束符
#define ST_TERM	        	SETTAB[basic+136]

//大小写
#define ST_DTTYPE	        SETTAB[basic+137]

//扫描方式
#define ST_REMOD	        SETTAB[basic+138]

//冗余裁决
#define ST_REDUN	        SETTAB[basic+139]

//音长
#define ST_BPLONG	        SETTAB[basic+140]

//音频
#define ST_FREQ	        	SETTAB[basic+141]

//音量
#define ST_SOUNDLEV	        SETTAB[basic+142]

//KB时钟
#define ST_CLK	        	SETTAB[basic+143]

//数据截取
#define ST_DRER	        	SETTAB[basic+144]

//一维反色
#define ST_RESERV1D	        SETTAB[basic+145]

//二维反色
#define ST_RESERV2D	        SETTAB[basic+146]

//一维多读
#define ST_APPEND1D	        SETTAB[basic+147]

//二维多读
#define ST_APPEND2D	        SETTAB[basic+148]

//解码休眠
#define ST_DCSLEEP	        SETTAB[basic+149]

//引擎休眠
#define ST_ENSLEEP	        SETTAB[basic+150]

//读取延时
#define ST_LAYSERDELAY	    SETTAB[basic+151]

//附加码读取延时
#define ST_ADD_DELAY	    SETTAB[basic+152]

//AUX
#define ST_AUX_MOD	        SETTAB[basic+153]

#if 0


//////////////////////////////////////////以上是条码相关设置
//////////////////////////////////////////
#define ST_NAK		SETTAB[basic+86]  //ack nak 握手
#define ST_CTS		SETTAB[basic+87]  //cts rts
#define ST_XON		SETTAB[basic+88]  //xon xoff
#define ST_COMM		SETTAB[basic+89]  //wait timeout
#define ST_COMSET		SETTAB[basic+90]  //串口命令类型a/b
#define ST_COMBEEP	SETTAB[basic+91]  //串口设置声音开关
#define ST_FUNKEY		SETTAB[basic+92]  //串口设置
#define ST_NUMLOCK	SETTAB[basic+93]  //小键盘锁定
#define ST_CAPIG		SETTAB[basic+94]  //大小写忽略
#define ST_AIM		SETTAB[basic+95]  //AIM
#define ST_CID		SETTAB[basic+96]  //code id
#define ST_BZCODE		SETTAB[basic+97]  //巴西条码转换
#define ST_TIPS		SETTAB[basic+98]  //开机提示
#define ST_RDBACK	SETTAB[basic+99] //条码倒置
#define ST_LENGTH		SETTAB[basic+100]  //display length before data
#define ST_IO0		SETTAB[basic+101] //IO控制0
#define ST_IO1		SETTAB[basic+102] //IO控制1 
#define ST_IO2		SETTAB[basic+103] //IO控制2
#define ST_IO3		SETTAB[basic+104] //IO控制3
#define ST_IRON		SETTAB[basic+105] //内置红外算法


// (保留至第127 共28个)	用于开关量设置
//#define ST_COMTYPA	SETTAB[basic+90]  //串口命令
//////////////////////////////////////////////////////////
#define	CASESET 128  //  11/100	   用于变量保存

#define  ST_TILTIME	SETTAB[CASESET+0]	   //连续读取时间间隔
#define  ST_TOUT	SETTAB[CASESET+1]	//NAK DELAYTIME 22.5s	3
#define  ST_START	SETTAB[CASESET+2]	//截取数据开始			2
#define  ST_END		SETTAB[CASESET+3]	//截取数据结束			2

#define  ST_LIML	SETTAB[CASESET+4]	//全局最短				2
#define  ST_MAXL	SETTAB[CASESET+5] 	//连续读取分割时间		3
#define  ST_CHADELY	SETTAB[CASESET+6]	//字符延时				3
#define  ST_BARDELY	SETTAB[CASESET+7]	 //字符串延时			3
#define  ST_BPLONG	SETTAB[CASESET+8]	 //
#define  ST_BPFREQ	SETTAB[CASESET+9]	 //			1
#define  ST_DCTIME	SETTAB[CASESET+10]	//解码器休眠1-255 S 
#define  ST_ENTIME	SETTAB[CASESET+11]	//引擎休眠时间1-255 S

#define  ST_IRLEVAL	SETTAB[CASESET+12]	//自动感应灵敏度1-255 S

// reser 12-15 

#define  READ_LENGTH CASESET+16

#define  ST_LEN_I25L	SETTAB[READ_LENGTH+0]//A  4-64	  4bit
#define  ST_LEN_I25M	SETTAB[READ_LENGTH+1]//A  4-64	  4bit
#define  ST_LEN_S25L	SETTAB[READ_LENGTH+2]//B  4-64
#define  ST_LEN_S25M	SETTAB[READ_LENGTH+3]//B  4-64
#define  ST_LEN_M25L	SETTAB[READ_LENGTH+4]//C  4-64
#define  ST_LEN_M25M	SETTAB[READ_LENGTH+5]//C  4-64
#define  ST_LEN_P25L	SETTAB[READ_LENGTH+6]//D  4-64
#define  ST_LEN_P25M	SETTAB[READ_LENGTH+7]//D  4-64
#define  ST_LEN_ITFL	SETTAB[READ_LENGTH+8]//E  4-64
#define  ST_LEN_ITFM	SETTAB[READ_LENGTH+9]//E  4-64
#define  ST_LEN_C11L	SETTAB[READ_LENGTH+10]//F  4-64
#define  ST_LEN_C11M	SETTAB[READ_LENGTH+11]//F  4-64 
#define  ST_LEN_CODL	SETTAB[READ_LENGTH+12]//G  4-64
#define  ST_LEN_CODM	SETTAB[READ_LENGTH+13]//G  4-64 
#define  ST_LEN_MSIL	SETTAB[READ_LENGTH+14]//H  4-64
#define  ST_LEN_MSIM	SETTAB[READ_LENGTH+15]//H  4-64
#define  ST_LEN_UKL		SETTAB[READ_LENGTH+16]//I  4-64
#define  ST_LEN_UKM		SETTAB[READ_LENGTH+17]//I  4-64
#define  ST_LEN_C39L	SETTAB[READ_LENGTH+18]//J  4-64
#define  ST_LEN_C39M	SETTAB[READ_LENGTH+19]//J  4-64
#define  ST_LEN_C93L	SETTAB[READ_LENGTH+20]//K  4-64
#define  ST_LEN_C93M	SETTAB[READ_LENGTH+21]//K  4-64
#define  ST_LEN_128L	SETTAB[READ_LENGTH+22]//L  4-64
#define  ST_LEN_128M	SETTAB[READ_LENGTH+23]//L  4-64
#define  ST_LEN_EXPL	SETTAB[READ_LENGTH+24]//M  4-64
#define  ST_LEN_EXPM	SETTAB[READ_LENGTH+25]//M  4-64

// reser 2*3 


#define  ST_POST READ_LENGTH+32 

#define  ST_PREAMBLE	SETTAB[ST_POST+0]//A  4-64
#define  ST_POSTAMBLE	SETTAB[ST_POST+1]//A  4-64


		//#define  ST_LEN_I25	SETTAB[READ_LENGTH+0]//A  4-64
 		//#define  ST_LEN_I25	SETTAB[READ_LENGTH+0]//A  4-64
		//#define  ST_LEN_S25	SETTAB[READ_LENGTH+1]//B  4-64
		//#define  ST_LEN_S25	SETTAB[READ_LENGTH+1]//B  4-64
		//#define  ST_LEN_M25	SETTAB[READ_LENGTH+2]//C  4-64
	//#define  ST_LEN_M25	SETTAB[READ_LENGTH+2]//C  4-64 
		//#define  ST_LEN_P25	SETTAB[READ_LENGTH+3]//D  4-64
	//#define  ST_LEN_P25	SETTAB[READ_LENGTH+3]//D  4-64 
		//#define  ST_LEN_ITF	SETTAB[READ_LENGTH+4]//E  4-64
	//#define  ST_LEN_ITF	SETTAB[READ_LENGTH+4]//E  4-64
		//#define  ST_LEN_C11	SETTAB[READ_LENGTH+5]//F  4-64
	//#define  ST_LEN_C11	SETTAB[READ_LENGTH+5]//F  4-64 
		//#define  ST_LEN_COD	SETTAB[READ_LENGTH+6]//G  4-64
	//#define  ST_LEN_COD	SETTAB[READ_LENGTH+6]//G  4-64 
		//#define  ST_LEN_MSI	SETTAB[READ_LENGTH+7]//H  4-64
	//#define  ST_LEN_MSI	SETTAB[READ_LENGTH+7]//H  4-64
		//#define  ST_LEN_UK	SETTAB[READ_LENGTH+8]//I  4-64
	//#define  ST_LEN_UK	SETTAB[READ_LENGTH+8]//I  4-64	
		//#define  ST_LEN_C39	SETTAB[READ_LENGTH+9]//J  4-640
	//#define  ST_LEN_C39	SETTAB[READ_LENGTH+9]//J  4-640	
		//#define  ST_LEN_C93	SETTAB[READ_LENGTH+10]//K  4-64
	//#define  ST_LEN_C93	SETTAB[READ_LENGTH+10]//K  4-64	
		//#define  ST_LEN_128	SETTAB[READ_LENGTH+11]//L  4-64
	//#define  ST_LEN_128	SETTAB[READ_LENGTH+11]//L  4-64	
		//#define  ST_LEN_EXP	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_EXP	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_C32	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_C32	SETTAB[READ_LENGTH+12]//M  4-64	 
		//#define  ST_LEN_UPCA	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_UPCA	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_UPCE	SETTAB[READ_LENGTH+12]//M  4-64
		//#define  ST_LEN_UPCE	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_EAN13	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_EAN13	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_EAM8	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_EAM8	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_RSS14	SETTAB[READ_LENGTH+12]//M  4-64	
	//#define  ST_LEN_RSS14	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_LIMITED	SETTAB[READ_LENGTH+12]//M  4-64	
		//#define  ST_LEN_LIMITED	SETTAB[READ_LENGTH+12]//M  4-64

// (保留至第227个参数) 用于批量设置
///////////////////////////////////////////////////////
#define	TABSET 228	  //13/28

#define  ST_PORT	SETTAB[TABSET+0] 
#define  ST_LANG	SETTAB[TABSET+1]
#define  ST_BAUTR	SETTAB[TABSET+2]
#define  ST_DATA	SETTAB[TABSET+3]
#define  ST_CHECK	SETTAB[TABSET+4]
#define  ST_STOP	SETTAB[TABSET+5]
#define  ST_TERM	SETTAB[TABSET+6]
#define  ST_DTTYPE	SETTAB[TABSET+7]
#define  ST_REMOD	SETTAB[TABSET+8]
#define  ST_REDUN	SETTAB[TABSET+9]
#define  ST_BEEP	SETTAB[TABSET+10]
#define  ST_FREQ	SETTAB[TABSET+11] 
#define  ST_CLK		SETTAB[TABSET+12]
#define  ST_DRER	SETTAB[TABSET+13]
#define  ST_RESERV	SETTAB[TABSET+14]	  //2 only decode normal  1 delcode reserved barcode both
#define  ST_DCSLEEP	SETTAB[TABSET+15]	//解码器休眠	0: 不休眠   1：读完立即休眠  2：10秒   3:60秒     4:10分钟    5：1小时   6:USER(1-255S)
#define  ST_ENSLEEP	SETTAB[TABSET+16]	//引擎休眠时间	0: 不休眠   1：读完立即休眠  2：10秒   3:60秒     4:10分钟    5：1小时   6:USER(1-255S)
// (保留至第255个参数) 用于批量设置
///////////////////////////////////////////////////////
#endif
unsigned char settingfunction(void);
extern unsigned char settingstate;
#endif

