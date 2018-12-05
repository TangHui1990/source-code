#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <sys/ioctl.h>

#include "xl_debug.h"
#include "control.h"

extern int BeepFd;
extern SerialportSetVoice eSerialPortStatus;
extern Terminator eTerminatorStatus;

int UartFd = -1;

/* 设置波特率 */
int set_baudrate(int fd, int nSpeed)
{
	struct termios settty;

	bzero(&settty, sizeof(settty));

	if (tcgetattr(fd, &settty) != 0)
	{
		XL_TRACE_DEBUG("Get Serial Attr Error!\n");

		return (-1);
	}

	switch ( nSpeed )  
    {
		case 1200:
			cfsetispeed(&settty, B1200);
			cfsetospeed(&settty, B1200);
			break;
        case 2400:
            cfsetispeed(&settty, B2400);
            cfsetospeed(&settty, B2400);
            break;
        case 4800:
            cfsetispeed(&settty, B4800);
            cfsetospeed(&settty, B4800);
            break;
        case 9600:
            cfsetispeed(&settty, B9600);
            cfsetospeed(&settty, B9600);
            break;
		case 14400:  /* 非标志波特率，不予支持 */
            //cfsetispeed(&settty, B14400);
            //cfsetospeed(&settty, B14400);
            break;
		case 19200:
            cfsetispeed(&settty, B19200);
            cfsetospeed(&settty, B19200);
            break;
		case 28800:  /* 非标志波特率，不予支持 */
            //cfsetispeed(&settty, B28800);
            //cfsetospeed(&settty, B28800);
            break;
		case 38400:
            cfsetispeed(&settty, B38400);
            cfsetospeed(&settty, B38400);
            break;
		case 57600:
            cfsetispeed(&settty, B57600);
            cfsetospeed(&settty, B57600);
            break;
        case 115200:
            cfsetispeed(&settty, B115200);
            cfsetospeed(&settty, B115200);
            break;
        default:
            cfsetispeed(&settty, B9600);
            cfsetospeed(&settty, B9600);
            break;
    }

	if ((tcsetattr(fd, TCSANOW, &settty)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
        return -1;
    }

	return (0);
}

/* 设置奇偶校验 */
int set_Paritycheck(int fd, char nEvent)
{
	struct termios settty;

	bzero(&settty, sizeof(settty));

	if (tcgetattr(fd, &settty) != 0)
	{
		XL_TRACE_DEBUG("Get Serial Attr Error!\n");

		return (-1);
	}

	switch ( nEvent )
    {
         case '0':  /*奇校验*/
             settty.c_cflag |= PARENB;				/*开启奇偶校验*/
             settty.c_iflag |= (INPCK | ISTRIP);	/*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
             settty.c_cflag |= PARODD;				/*启用奇校验(默认为偶校验)*/
             break;
         case 'E':/*偶校验*/
             settty.c_cflag |= PARENB; 				/*开启奇偶校验  */
             settty.c_iflag |= ( INPCK | ISTRIP);	/*打开输入奇偶校验并去除字符第八个比特*/
             settty.c_cflag &= ~PARODD;				/*启用偶校验*/
             break;
         case 'N': /*无奇偶校验*/
             settty.c_cflag &= ~PARENB;
             break;
    }

	if ((tcsetattr( fd, TCSANOW, &settty)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
        return -1;
    }

	return (0);
}

int set_databit(int fd, int nBits)
{
	struct termios settty;

	bzero(&settty, sizeof(settty));

	if (tcgetattr(fd, &settty) != 0)
	{
		XL_TRACE_DEBUG("Get Serial Attr Error!\n");

		return (-1);
	}

	/*数据位选择*/   
     switch ( nBits )
     {
         case 7:
             settty.c_cflag |= CS7;
             break;
         case 8:
             settty.c_cflag |= CS8;
             break;
     }

	if ((tcsetattr( fd, TCSANOW, &settty)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
        return -1;
    }

	return (0);
	 
}

int set_stopbit(int fd, int nStop)
{
	struct termios settty;

	bzero(&settty, sizeof(settty));

	if (tcgetattr(fd, &settty) != 0)
	{
		XL_TRACE_DEBUG("Get Serial Attr Error!\n");

		return (-1);
	}
	
    if ( nStop == 1)				/*设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB*/
    {
        settty.c_cflag &= ~CSTOPB;	/*默认为一位停止位； */
    }
    else if ( nStop == 2)
    {
        settty.c_cflag |= CSTOPB;	/*CSTOPB表示送两位停止位*/
    }

	if ((tcsetattr( fd, TCSANOW, &settty)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
        return -1;
    }

	return (0);
}

/*
 *  https://blog.csdn.net/eason_will_be_good/article/details/43762919
 * 	options.c_cc[VSTOP]    = 0x22;
 *	options.c_cc[VSTART]   = 0x33;
 * 	options.c_iflag |= IXON|IXOFF; 
 * 
 * 	options.c_cc[VSTOP]    = 0x22; ------> 要求对端停止发送数据
 * 	options.c_cc[VSTART]   = 0x33; ------> 告诉对端可以发送数据
 * 	options.c_iflag |= IXON|IXOFF; //使用软件流控制
 *
 */
int set_flowcontrol(int fd, int nflowControl)
{
	struct termios settty;

	bzero(&settty, sizeof(settty));

	if (tcgetattr(fd, &settty) != 0)
	{
		XL_TRACE_DEBUG("Get Serial Attr Error!\n");

		return (-1);
	}

	if (nflowControl == 0)   		/* 关闭流控 */
	{
		settty.c_cflag &= ~CRTSCTS;
	}
	else if(nflowControl == 1)
	{
		settty.c_cflag |= CRTSCTS;  /* 开启流控 */
	}

	if ((tcsetattr( fd, TCSANOW, &settty)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
		
        return -1;
    }

	return (0);
}

int set_serial(int fd, int nSpeed, int nBits, char nEvent, int nStop, int nflowControl)
{
    struct termios newttys1,oldttys1;

     /*保存原有串口配置*/
     if (tcgetattr(fd, &oldttys1) != 0) 
     {
          XL_TRACE_DEBUG("Setupserial 1");
          return -1;
     }
	 
     bzero(&newttys1, sizeof(newttys1));
     newttys1.c_cflag |= (CLOCAL|CREAD ); /*CREAD 开启串行数据接收，CLOCAL并打开本地连接模式*/

     newttys1.c_cflag &= ~CSIZE;/*设置数据位*/
     /*数据位选择*/   
     switch ( nBits )
     {
         case 7:
             newttys1.c_cflag |= CS7;
             break;
         case 8:
             newttys1.c_cflag |= CS8;
             break;
     }
     /*设置奇偶校验位*/
     switch ( nEvent )
     {
         case '0':  /*奇校验*/
             newttys1.c_cflag |= PARENB;/*开启奇偶校验*/
             newttys1.c_iflag |= (INPCK | ISTRIP);/*INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特  */
             newttys1.c_cflag |= PARODD;/*启用奇校验(默认为偶校验)*/
             break;
         case 'E':/*偶校验*/
             newttys1.c_cflag |= PARENB; /*开启奇偶校验  */
             newttys1.c_iflag |= ( INPCK | ISTRIP);/*打开输入奇偶校验并去除字符第八个比特*/
             newttys1.c_cflag &= ~PARODD;/*启用偶校验*/
             break;
         case 'N': /*无奇偶校验*/
             newttys1.c_cflag &= ~PARENB;
             break;
     }
     /*设置波特率*/
    switch ( nSpeed )  
    {
        case 2400:
            cfsetispeed(&newttys1, B2400);
            cfsetospeed(&newttys1, B2400);
            break;
        case 4800:
            cfsetispeed(&newttys1, B4800);
            cfsetospeed(&newttys1, B4800);
            break;
        case 9600:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
        case 115200:
            cfsetispeed(&newttys1, B115200);
            cfsetospeed(&newttys1, B115200);
            break;
        default:
            cfsetispeed(&newttys1, B9600);
            cfsetospeed(&newttys1, B9600);
            break;
    }
     /*设置停止位*/
    if ( nStop == 1)/*设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB*/
    {
        newttys1.c_cflag &= ~CSTOPB;/*默认为一位停止位； */
    }
    else if ( nStop == 2)
    {
        newttys1.c_cflag |= CSTOPB;/*CSTOPB表示送两位停止位*/
    }

	if (nflowControl == 0)   			/* 关闭流控 */
	{
		newttys1.c_cflag &= ~CRTSCTS;
	}
	else if (nflowControl == 1)
	{
		newttys1.c_cflag |= CRTSCTS;  	/* 开启流控 */
	}

    /*设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时*/
    newttys1.c_cc[VTIME] = 0;/*非规范模式读取时的超时时间；*/
    newttys1.c_cc[VMIN]  = 0; /*非规范模式读取时的最小字符数*/
    tcflush(fd ,TCIFLUSH);/*tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

     /*激活配置使其生效*/
    if ((tcsetattr( fd, TCSANOW, &newttys1)) != 0)
    {
        XL_TRACE_DEBUG("com set error");
        return -1;
    }

    return 0;
}

int uart_read(char *cbuf, int ilen)
{
	int iRet;

	iRet = read(UartFd, cbuf, ilen);
	if (iRet > 0)
	{
		if (SERIALPORTSETVOICE_OPEN == eSerialPortStatus)  /* 串口设置提示音 */
		{
			iRet = ioctl(BeepFd, DECODESUCCESS_SOUND, (long)COMSETPROMPT_SOUND);
		
			XL_RETURN(iRet < 0, -1, "ioctl error!");
		}
		
		return (iRet);
	}

	return (0);
}

int uart_write(char *cbuf, int iLen)
{
	int iRet;
	char *changeline = "\r\n";
	char space[6] = {'\0'};
	unsigned char cTerminal;

	iRet = write(UartFd, cbuf, iLen);
	XL_RETURN(iRet < 0, -1, "Uart write error!");

	if (eTerminatorStatus == TERMINATOR_TAB)
	{
		cTerminal = 0x0D;
		iRet = write(UartFd, &cTerminal, sizeof(cTerminal));
		XL_RETURN(iRet < 0, -1, "Uart write error!");

		cTerminal = 0x0A;
		iRet = write(UartFd, &cTerminal, sizeof(cTerminal));
		XL_RETURN(iRet < 0, -1, "Uart write error!");
	}
	else if(eTerminatorStatus == TERMINATOR_ENTER)
	{
		cTerminal = 0x0D;
		iRet = write(UartFd, &cTerminal, sizeof(cTerminal));
		XL_RETURN(iRet < 0, -1, "Uart write error!");
	}
	else if(eTerminatorStatus == TERMINATOR_SPACE)
	{
		cTerminal = 0x20;
		iRet = write(UartFd, &cTerminal, sizeof(cTerminal));
		XL_RETURN(iRet < 0, -1, "Uart write error!");
	}
	else if(eTerminatorStatus == TERMINATOR_RETURN)
	{
		cTerminal = 0x0A;
		iRet = write(UartFd, &cTerminal, sizeof(cTerminal));
		XL_RETURN(iRet < 0, -1, "Uart write error!");
	}

	//write(UartFd, changeline, sizeof(changeline));

	return (0);
}

int uart_init(void)
{
	int iRet;
	
//	UartFd = open("/dev/ttyGS0", O_RDWR|O_NOCTTY|O_NDELAY);
//	XL_RETURN(UartFd < 0, -1, "Open /dev/ttyGS0 error!\n");

	UartFd = open("/dev/ttyS1", O_RDWR|O_NOCTTY|O_NDELAY);
	XL_RETURN(UartFd < 0, -1, "Open /dev/ttyS1 error!\n");

	iRet = set_serial(UartFd, 115200, 8, 'N', 1, 0);
	XL_RETURN(iRet < 0, -1, "Serial set error!\n");

	return (0);
}




