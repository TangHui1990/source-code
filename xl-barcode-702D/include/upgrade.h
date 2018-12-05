#ifndef __UPGRADE_H__
#define __UPGRADE_H__

typedef struct XL_VirtualPortUpgradePackage
{
	unsigned char FrameHead;  	/* 帧头 */
	unsigned char FuncWord;		/* 功能字 */
	unsigned int DataLen;		/* 数据长度 */
	unsigned char Data[64];		/* 数据 */
	unsigned int CheckSum;		/* 校验 */
	unsigned char RepeatFlag;	/* 数据重发标志 */
	unsigned char FinishFlag;	/* 数据传输完成标志 */
}VirtualPortUpgradePackage;


extern int XL_UpgradeForMassStorageRunning(void);


#endif
