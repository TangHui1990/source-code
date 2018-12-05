#define GPHCON     (*(volatile unsigned long*)0x56000070)
#define ULCON0     (*(volatile unsigned long*)0x50000000)
#define UCON0      (*(volatile unsigned long*)0x50000004)
#define UBRDIV0    (*(volatile unsigned long*)0x50000028)
#define UTRSTAT0   (*(volatile unsigned long*)0x50000010)
#define UTXH0      (*(volatile unsigned long*)0x50000020)
#define URXH0      (*(volatile unsigned long*)0x50000024)

#define PCLK 50000000
#define BAUD 115200

void uart_init(void)
{
	/*1. 配置引脚功能 GPH2, GPH3*/
	GPHCON &= ~(0xf0);
	GPHCON |= (0xa0);

	/*2.1 设置数据格式*/
	ULCON0 |= (0x3 << 0);

	/*2.2 设置工作模式*/
	//UCON0 = 0b0101;        //Interrupt request or polling mode
	UCON0 = 0b1010;          //DMA0

	/*2.3 设置波特率*/
	UBRDIV0 = (int)((PCLK/(BAUD*16) -1));
}

/*串口发送函数*/
void gboot_putc(unsigned char ch)
{
	while (!(UTRSTAT0 & (1 << 2)))  //不为空时发送
	{
		continue;
	}

	UTXH0 = ch;
}

/*串口接收函数*/
unsigned char gboot_getc(void)
{
	unsigned char ret;

	while (!(UTRSTAT0 & (1 << 0))) //不为空时接收
	{
		continue;
	}

	ret = URXH0;

	if ((ret == 0x0d) || (ret == 0x0a)) //回车换行
	{
		gboot_putc(0x0d);
		gboot_putc(0x0a);
	}
	else
	{
		gboot_putc(ret);
	}

	return (ret);
}
