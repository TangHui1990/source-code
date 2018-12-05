//#define GPBCON (volatile unsigned long*)0xA0000010 
//#define GPBDAT (volatile unsigned long*)0xA0000014
#define GPBCON (volatile unsigned long*)0x56000010 
#define GPBDAT (volatile unsigned long*)0x56000014

void led_init(void)
{
	*(GPBCON) = 0x400;
}

void led_on(void)
{
	*(GPBDAT) = 0x0;
}

void led_off(void)
{
	*(GPBDAT) = 0x7ff;
}