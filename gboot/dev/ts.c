#define ADCCON (*(volatile unsigned long*)0x58000000)
#define ADCTSC (*(volatile unsigned long*)0x58000004)
#define ADCDAT0 (*(volatile unsigned long*)0x5800000C)
#define ADCDAT1 (*(volatile unsigned long*)0x58000010)

#define INTMSK    (*(volatile unsigned long*)0x4A000008)
#define INTSUBMSK (*(volatile unsigned long*)0x4A00001C)
#define INTPND    (*(volatile unsigned long*)0x4A000010)
#define SUBSRCPND (*(volatile unsigned long*)0x4A000018)
#define SRCPND    (*(volatile unsigned long*)0x4A000000)

int xdata;
int ydata;

void ts_init(void)
{
	/*1.设置AD转换时钟*/
	ADCCON = (1 << 14) | (49 << 6);   //1M

	/*2.设置中断屏蔽位*/
	INTMSK &= ~(1 << 31);
	INTSUBMSK &= ~(1 << 9);

	/*3.进入等待中断的模式*/
	ADCTSC = 0xd3;
}

void tc_handler(void)
{
	/*1.启动xy坐标自动转换*/
	ADCTSC |= (1 << 2);
	ADCCON |= (1 << 0);

	/*2.等待转换完成*/
	while (!((ADCCON&(0x1<<15))))
	{
		continue;
	}

	/*3.获取坐标*/
	xdata = ADCDAT0 & 0x3ff;
	ydata = ADCDAT1 & 0x3ff;

	/*4.清除按下中断*/
	SUBSRCPND |= (1 << 9);
	SRCPND    |= (1 << 31);
	INTPND    |= (1 << 31);

	/*5.进入等待弹起中断*/
	ADCTSC = 0xd3;
	ADCTSC |= (1 << 8);

	while (!((SUBSRCPND)&(0x1<<9)))
	{
		continue;
	}

	/*6.清除弹起中断*/
	SUBSRCPND |= (1 << 9);
	SRCPND    |= (1 << 31);
	INTPND    |= (1 << 31);

	printf("xdata = %d, ydata = %d\r\n", xdata, ydata);

	/*7.再次进入等待按下中断的状态*/
	ADCTSC = 0xd3;
}
