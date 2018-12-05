#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <control.h>
#include <xl_debug.h>
#include <systemseting.h>
#include <device.h>

#include <videodev2.h>


#define PREEMPT_CTROL 1
int fd;
int BeepFd = -1;
int buttons_fd = -1;
unsigned char SetModeFlag = 0;
unsigned int iVoiceCtrl = 0x0;

static int Beep_Init(void)
{
    BeepFd = open("/dev/mypwm", O_RDWR);
	XL_RETURN(BeepFd < 0, -1, "open /dev/mypwm error!\n");
	
	return 0;
}

int main(int argc, char *argv[])
{
	int ret;
	
//#ifdef CONFIG_SOC_CAMERA_AR0144
	buttons_fd = open("/dev/jz-button", O_RDONLY|O_NONBLOCK);
	XL_ASSERT(buttons_fd < 0, "Open /dev/jz-button error!\n");
//#endif

	ret = Beep_Init();
	XL_RETURN(ret<0, -1, "Beep_Init error!");

	if (BOOTSOUND_OPEN == eBootSoundSetStatus)  /* 开机提示音 */
	{
		ioctl(BeepFd, BOOTPROMPT_SOUND, 0);
	}

	//InitDebufInfo();

#ifdef CONFIG_SOC_CAMERA_GC0308
	//ioctl(BeepFd, JZ_LED_ON, 0);
#endif

	//if (eInterfaceStatus == INTERFACE_VCP)
	//{
		ret = uart_init();
		
		//XL_RETURN(ret<0, -1, "uart_init error!");
	//}

	Init_Setcode();
	
	//if (eInterfaceStatus == INTERFACE_HID)
	//{
		ret = Hid_Init();
		
		//XL_RETURN(ret<0, -1, "Hid_Init error!");
	//}
	
#ifdef CONFIG_SOC_CAMERA_GC0308
	initCamera(PREVIEW_WIDTH, PREVIEW_HEIGHT, V4L2_PIX_FMT_YUYV);
#endif

#ifdef CONFIG_SOC_CAMERA_AR0144 || CONFIG_SOC_CAMERA_MT9V022
	camera_init();
#endif

	
#ifdef CONFIG_SOC_CAMERA_AR0144
	XL_AR0144_QrCodeDealWith();
#endif

#ifdef CONFIG_SOC_CAMERA_GC0308
	XL_GC0308_QrCodeDealWith();
#endif

#ifdef CONFIG_SOC_CAMERA_MT9V022
	buttons_fd = open("/dev/jz-button", O_RDONLY|O_NONBLOCK);
	XL_ASSERT(buttons_fd < 0, "Open /dev/jz-button error!\n");
	
	XL_MT9V022_QrCodeDealWith();
#endif

#ifdef CONFIG_SOC_CAMERA_AR0144 || CONFIG_SOC_CAMERA_MT9V022
	camera_exit();
#endif

	Hid_Exit();

#ifdef CONFIG_SOC_CAMERA_AR0144
	close(BeepFd);
	close(buttons_fd);
#endif

	return 0;
}


