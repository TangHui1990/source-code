#define INTMSK       (volatile unsigned long*)0x4A000008
#define EINTMASK     (volatile unsigned long*)0x560000a4
#define EINTPEND     (volatile unsigned long*)0x560000a8
#define SRCPND       (volatile unsigned long*)0x4A000000
#define INTPND       (volatile unsigned long*)0x4A000010
#define INTOFFSET           (volatile unsigned long *)0x4A000014

void init_irq(void)
{
	// ��EINTMASK�Ĵ�����ʹ��EINT8_23�о����8,11,13,14��ע������������������һ������ǰ��
	*(EINTMASK) &= (~(1 << 8)) & (~(1 << 11)) & (~(1 << 13)) & (~(1 << 14));
	// EINT8_23ʹ��
	*(INTMSK) &= ~(1 << 5);
	
	__asm__(
		/*��һ�׶εĴ��������ǰ��жϸ��ر��ˣ���������Ҫ����*/
		"mrs r0, cpsr\n"
		"bic r0, r0, #0x80\n"
		"msr cpsr_c, r0\n"
		:
		:
		);
}

void handle_int(void)
{
	/*��ȡ�ж�Դ��ֵ*/
	//unsigned long value = *(EINTPEND) & ((1<<8)|(1<<11)|(1<<13)|(1<<14));
	/*��ȡ�����жϵ�Դ*/
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
	
	/* �ж����(д1���)*/
	*(EINTPEND) = value;
	*(SRCPND) = 1 << 5;
	*(INTPND) = 1 << 5;
}
