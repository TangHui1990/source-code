/*
 * HDSD.h
 *
 *  Created on: 2018年01月19日
 *  release 0.2.1-HDSD-SDK-20180209
 *      Author: Cloud
 */

#ifndef _HDSD_H_
#define _HDSD_H_

/* return value */
#define	RET_SUCCESS                 0
#define	RET_ERR_PARAM               -1
#define	RET_FAILED                  -2

/* code type define */
#define SET_CODETYPE_EAN13          1
#define SET_CODETYPE_EAN8           2
#define SET_CODETYPE_UPCA           3
#define SET_CODETYPE_UPCE           4
#define SET_CODETYPE_C128           5
#define SET_CODETYPE_C39            6
#define SET_CODETYPE_ITF25          7
#define SET_CODETYPE_C93            9
#define SET_CODETYPE_NL128          11
#define SET_CODETYPE_EAN128         13
#define SET_CODETYPE_MATRIX25       16
#define SET_CODETYPE_AIM128         17
#define SET_CODETYPE_PDF417		    32
#define SET_CODETYPE_QR			    33
#define SET_CODETYPE_DATAMATRIX	    34

/* SetBar paramter define */
#define SET_CLASS_ENABLE            0x10001000
#define SET_CLASS_PROPERTY          0x10001001

// 条码使能开关
#define SET_VAL_DISABLE             0           // 关闭使能
#define SET_VAL_ENABLE              1           // 开启使能

// 通用码制属性，种类有:CodeBar\ITF25\Matrix25\NEC25
//|_________________________________________________________________________________________
//| bit0     | bit1     | bit2     | bit3     | bit4     | bit5     | bit6     | bit7      |
//|__________|__________|__________|__________|__________|__________|__________|___________|
//| checksum | undef    | undef    | strip    | undef    | undef    | undef    | undef     |
//|__________|__________|__________|__________|__________|__________|__________|___________|
#define SET_VAL_NO_CHECK            0x00        // Disable checksum checking (default)
#define SET_VAL_CHECKSUM            0x01        // Enable checksum checking
#define SET_VAL_STRIP               0x08        // Strip the checksum from the result string(与checksum属性组合使用)

// C39专用属性
//|_________________________________________________________________________________________
//| bit0     | bit1     | bit2     | bit3     | bit4     | bit5     | bit6     | bit7      |
//|__________|__________|__________|__________|__________|__________|__________|___________|
//| checksum | undef    | undef    | strip    | fullASC  | undef    | undef    | undef     |
//|__________|__________|__________|__________|__________|__________|__________|___________|
#define SET_VAL_C39_NO_CHECK        0x00        // Disable checksum checking (default)
#define SET_VAL_C39_CHECKSUM        0x01        // Enable checksum checking
#define SET_VAL_C39_STRIP           0x08        // Strip the checksum from the result string(与checksum属性组合使用)
#define SET_VAL_C39_FULLASCII       0x10        // Full ASCII

// EAN/UPC专用属性
//|_________________________________________________________________________________________
//| bit0     | bit1     | bit2     | bit3     | bit4     | bit5     | bit6     | bit7      |
//|__________|__________|__________|__________|__________|__________|__________|___________|
//| checksum | undef    | undef    | strip    | supp     | supp2    | supp5    | expan     |
//|__________|__________|__________|__________|__________|__________|__________|___________|
#define SET_VAL_EAN_NO_CHECK        0x00        // Disable checksum checking (无效)
#define SET_VAL_EAN_CHECKSUM        0x01        // Enable checksum checking (default)
#define SET_VAL_EAN_STRIP           0x08        // Strip the checksum from the result string(无效)
#define SET_VAL_EAN_SUPPLEMENTAL    0x10        // Enable supplemental decoding
#define SET_VAL_EAN_SUPPLEMENTAL2   0x20        // Enable 2 digit supplemental symbol
#define SET_VAL_EAN_SUPPLEMENTAL5   0x40        // Enable 5 digit supplemental symbol
#define SET_VAL_EAN_EXPANSION       0x80        // Enable EAN8 and UPC8 expand to EAN13 and UPC13

/*
 * HD_iInit
 *
 * input param ：
 *               InBuf -- Verify string
 * return :
 *          =-1 is parameter error
 *          =-2 is invalid error
 *          = 0 is Ok
 */
int  HD_iInit(char *InBuf);

/*
 * HD_iDecodeBar
 *
 * input param ：
 *               nWidth    -- bmp width
 *               nHeight   -- bmp height
 *               pBmpBit   -- bmp buffer
 *               nDataSize -- pData size
 * output param:
 *               nType     -- code type
 *               pData     -- barcode info
 * return :
 *          =-1 is parameter error
 *          =-2 is invalid error
 *          = 0 is decode end
 *          > 0 is barcode length
 */
int  HD_iDecodeBar(int nWidth, int nHeight, char *pBmpBit, int *nType, char *pData, int nDataSize);

/*
 * HD_iSetBar
 *
 * input param ：
 *               nClass    -- SET_CLASS_ENABLE / SET_CLASS_PROPERTY
 *               nType     -- code type
 *               nVal      -- property value
 * output param:
 * return :
 *          =-1 is parameter error
 *          =-2 is invalid error
 *          = 0 is set success
 */
int HD_iSetBar(int nClass, int nType, int nVal);

/*
 * HD_iGetVer
 *
 * input param ：
 *               nVerSize  -- pVer size, >= 36 Bytes
 * output param:
 *               pVer      -- output Version
 * return :
 *          =-1 is parameter error
 *          =-2 is invalid error
 *          >0  pVer length
 */
int  HD_iGetVer(char* pVer, int nVerSize);

#endif /* _HDSD_H_ */

