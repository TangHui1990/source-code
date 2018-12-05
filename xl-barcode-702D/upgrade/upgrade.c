#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <upgrade.h>
#include <xl_debug.h>
#include <device.h>

FILE *pFile = NULL;

int XL_UpgradeAppForMassStorage(void)
{
	int ret;

	ret = system("cd /");
	XL_RETURN(ret < 0, -1, "cd / error!\n");

	if (access("/gadget/mass_storage/finish", F_OK) == 0)
	{
		ret = system("rm /gadget/mass_storage/finish");
		XL_RETURN(ret < 0, -1, "rm /gadget/mass_storage/finish error!\n");
	}
	
	while ( access("/gadget/mass_storage/vfat_mount_point/camera_test", F_OK) != 0 )
	{
		sleep(1);

		system("ls /gadget/mass_storage/vfat_mount_point/");

		XL_TRACE_DEBUG("wait...\n");
		continue;
	}

	ret = system("cp /gadget/mass_storage/vfat_mount_point/camera_test /");	
	XL_RETURN(ret < 0, -1, "cp /gadget/mass_storage/vfat_mount_point/camera_test / error!\n");

	ret = system("sync");
	XL_RETURN(ret < 0, -1, "sync error!\n");

	ret = system("rm /gadget/mass_storage/vfat_mount_point/camera_test");
	XL_RETURN(ret < 0, -1, "rm /gadget/mass_storage/vfat_mount_point/camera_test error!\n");
	
	return (0);
}

static int XL_HidToVirtualSerialPort(void)
{
	int ret;
	ret = system("cd /");
	XL_RETURN(ret < 0, -1, "cd / error!");

	ret = system("source XL_HidToVirtualSerialPort.sh");
	XL_RETURN(ret < 0, -1, "source XL_HidToVirtualSerialPort.sh error!");
	
	return (0);
}

static int XL_VirtualSerialPortToHid(void)
{
	int ret;
	
	ret = system("cd /");
	XL_RETURN(ret < 0, -1, "cd / error!");

	ret = system("source XL_VirtualSerialPortToHid.sh");
	XL_RETURN(ret < 0, -1, "source XL_VirtualSerialPortToHid.sh error!");
	
	return (0);
}

static int XL_HidToMassStorage(void)
{
	int ret;

	ret = system("cd /");
	XL_RETURN(ret < 0, -1, "cd / error!");

	ret = system("source XL_HidToMassStorage.sh");
	XL_RETURN(ret < 0, -1, "source XL_HidToMassStorage.sh error!");
	
	return (0);
}

static int XL_MassStorageToHid(void)
{
	int ret;

	ret = system("cd /");
	XL_RETURN(ret < 0, -1, "cd / error!");

	ret = system("source XL_MassStorageToHid.sh");
	XL_RETURN(ret < 0, -1, "source XL_MassStorageToHid.sh error!");
	
	return (0);
}


static unsigned char XL_Checkcrc8(unsigned char *ptr,  unsigned char len)
{
    unsigned char i, crc8;

    crc8 = ptr[0];
	
    for (i=1; i < len; i++) 
	{
        crc8 ^= ptr[i];
    }
    
	return crc8;
}


static int XL_PackSendData(VirtualPortUpgradePackage **pSendData)
{	
	XL_ASSERT(pSendData == NULL, "pSendData is NULL!\n");

	(*pSendData)->FrameHead = '0xAA';
	(*pSendData)->FuncWord = (char)0x02;
	(*pSendData)->DataLen = 4;
	strncpy((*pSendData)->Data, "repeat",64);
	(*pSendData)->CheckSum =  XL_Checkcrc8((*pSendData)->Data, 64);
	(*pSendData)->RepeatFlag = 1;

	return (0);
}


static int XL_UnpackData(unsigned char *pSrcData, unsigned char **pDstData, unsigned int *iDatalen)
{
	int iRepeatFlag = 0;
	int ret;
	
	XL_ASSERT(pSrcData == NULL || pDstData == NULL || iDatalen == NULL, "pSrcData or pDstData or iDatalen is NULL!\n");
	
	VirtualPortUpgradePackage *ReceivePackageData = (VirtualPortUpgradePackage*)pSrcData;
	VirtualPortUpgradePackage *SendPackageData = (VirtualPortUpgradePackage *)malloc(sizeof(VirtualPortUpgradePackage));
	XL_RETURN(SendPackageData == NULL, -1, "malloc VirtualPortUpgradePackage error");

	if(ReceivePackageData->FrameHead != 0xAA)
	{
		XL_TRACE_DEBUG("Protocol Data FrameHead Error!");

		iRepeatFlag = 1;
	}

	if (ReceivePackageData->FuncWord != 0x01)
	{
		XL_TRACE_DEBUG("Protocol Data FuncWord Error!");

		iRepeatFlag = 1;
	}

	if (XL_Checkcrc8(ReceivePackageData->Data, ReceivePackageData->DataLen) != ReceivePackageData->CheckSum)
	{
		XL_TRACE_DEBUG("Protocol Data Data Error!");

		iRepeatFlag = 1;
	}

	if (iRepeatFlag == 1)
	{
		ret = XL_PackSendData(&SendPackageData);
		if (ret < 0)
		{
			XL_TRACE_DEBUG("Function XL_PackSendData Error!");

			goto XLUnpackDataException;
		}

		ret = uart_write(SendPackageData, sizeof(SendPackageData));
		if (ret < 0)
		{
			XL_TRACE_DEBUG("uart_write Send Repeat Send Package Error!");

			goto XLUnpackDataException;
		}
	}
	else
	{
		*pDstData = ReceivePackageData->Data;
		*iDatalen = ReceivePackageData->DataLen;
	}

	free(SendPackageData);
	
	return (0);

XLUnpackDataException:
	free(SendPackageData);
	return (-1);
	
}


#define UPGRADE_FILE "/app"
static int XL_UpgradeInit(void)
{
	pFile = fopen(UPGRADE_FILE, "wb+");
	XL_RETURN(pFile == NULL, -1, "fopen update file error!");
	
	return (0);
}

static int XL_ReceiveProtocolData(void)
{
	int ret;
	VirtualPortUpgradePackage pVirtualPortUpgradePackage;
	unsigned int iDataLen;

	char *pDstData = (char *)malloc(64);
	XL_RETURN(pDstData == NULL, -1, "malloc pDstData error!");

	while ( 1 )
	{
		ret = uart_read(&pVirtualPortUpgradePackage, sizeof(VirtualPortUpgradePackage));
		if  ( ret > 0 )
		{
			if ( pVirtualPortUpgradePackage.FinishFlag = 0)
			{
				ret = XL_UnpackData(&pVirtualPortUpgradePackage, &pDstData, &iDataLen);
				XL_RETURN(ret < 0, -1, "XL_UnpackData error!\n");

				ret = fwrite(pDstData, 1, iDataLen, pFile);
				XL_RETURN(ret != iDataLen, -1, "fwrite %u failed, s32Ret %d", iDataLen, ret);
				
				memset(pDstData, '\0', 64);
			}
			else  /* 数据接收完成 */
			{				
				break;
			}
			
		}
	}

	fclose(pFile);			
	free(pDstData);
	
	return (0);
}

int XL_UpgradeForMassStorageRunning(void)
{
	int ret;

	Hid_Exit();
	
	ret = XL_HidToMassStorage();
	XL_RETURN(ret < 0, -1, "Function Hid To Mass Storage error!\n");

	ret = XL_UpgradeAppForMassStorage();
	XL_RETURN(ret < 0, -1, "Upgrade App For mass Storage error!");

	ret = system("source /XL_UpgradeRunning.sh");
	XL_RETURN(ret < 0, -1, "source /XL_UpgradeRunning.sh error");
	
	return (0);
}


