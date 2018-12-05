#define INTMSK       (volatile unsigned long*)0x4A000008
#define EINTMASK     (volatile unsigned long*)0x560000a4
#define EINTPEND     (volatile unsigned long*)0x560000a8
#define SRCPND       (volatile unsigned long*)0x4A000000
#define INTPND       (volatile unsigned long*)0x4A000010
#define INTOFFSET           (volatile unsigned long *)0x4A000014

void init_irq(void)
{
	// 在EINTMASK寄存器中使能EINT8_23中具体的8,11,13,14，注意这条语句必须在下面一条语句的前面
	*(EINTMASK) &= (~(1 << 8)) & (~(1 << 11)) & (~(1 << 13)) & (~(1 << 14));
	// EINT8_23使能
	*(INTMSK) &= ~(1 << 5);
	
	__asm__(
		/*第一阶段的代码中我们把中断给关闭了，所以这里要开启*/
		"mrs r0, cpsr\n"
		"bic r0, r0, #0x80\n"
		"msr cpsr_c, r0\n"
		:
		:
		);
}

void handle_int(void)
{
	/*读取中断源的值*/
	//unsigned long value = *(EINTPEND) & ((1<<8)|(1<<11)|(1<<13)|(1<<14));
	/*读取产生中断的源*/
	unsigned long value = *(INTOFFSET);
	switch ( value )
	{
		case (1 << 8): //K1
			led_on();
			break;
			
		case (1 << 11): //K2
			led_off();
			break;
			
		case (1 << 13): //K3
			led_on();
			break;
		
		case (1 << 14): //K4
			led_off();
			break;

		case 4:
			int_issue();
			break;
		case 31:
			tc_handler();
		break;
			
		default:
			break;
	}
	
	/* 中断清除(写1清除)*/
	*(EINTPEND) = value;
	*(SRCPND) = 1 << 5;
	*(INTPND) = 1 << 5;
}
