#define NFDATA (*(volatile unsigned char*)0x4E000010)
#define NFCONT (*(volatile unsigned long*)0x4E000004)
#define NFSTAT (*(volatile unsigned char*)0x4E000020)
#define NFCMMD (*(volatile unsigned char*)0x4E000008)
#define NFADDR (*(volatile unsigned char*)0x4E00000C)
#define NFCONF (*(volatile unsigned long*)0x4E000000)

#define TACLS 1 
#define TWRPH0 2
#define TWRPH1 1

/*ѡ��оƬ*/
void select_chip(void)
{
	NFCONT &= ~(1 << 1);
}

/*ȡ��ѡ��оƬ*/
void deselect_chip(void)
{
	NFCONT |= (1 << 1);
}

/*���RnB*/
void clear_RnB(void)
{
	NFSTAT |= (1 << 2);
}

/*��������*/
void send_cmd(unsigned cmd)
{
	NFCMMD = cmd;
}

/*���͵�ַ*/
void send_addr(unsigned addr)
{
	NFADDR = addr;
}

/*�ȴ�RnB*/
void wait_RnB(void)
{
	while (!(NFSTAT&(1<<2)))
	{
		continue;
	}
}

void nand_reset(void)
{
	/*ѡ��flash*/
	select_chip();

	/*���RnB*/
	clear_RnB();

	/*����0xff����*/
	send_cmd(0xff);

	/*�ȴ�RnB*/
	wait_RnB();

	/*ȡ��ѡ��flash*/
	deselect_chip();
}

/*nand flash init*/
void nandflash_init(void)
{
	/*��ʼ��NFCONF*/
	NFCONF = (TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4);

	/*��ʼ��NFCONT*/
	NFCONT = (1<<0) | (1<<1);

	/*��λ*/
	nand_reset();
}

/*nand flash read*/
void NF_PageRead(unsigned long addr, unsigned char *buff)
{
	int i;

	/*ѡ��nandflashоƬ*/
	select_chip();

	/*���RnB*/
	clear_RnB();

	/*��������0x00*/
	send_cmd(0x00);

	/*�����е�ַ��2������*/
	send_addr(0x00); /*�е�ַ����ҳ��ƫ�ƣ���ҳ��û��ҳ��ƫ�ƣ���Ϊ0*/
	send_addr(0x00);

	/*�����е�ַ, 3������*/
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);

	/*��������0x30*/
	send_cmd(0x30);

	/*�ȴ�RnB*/
	wait_RnB();

	/*��ȡ����*/
	for ( i = 0; i < 2048; i++) /*һ��ҳ��2K��byte��������2048*/
	{
		buff[i] = NFDATA;
	}

	/*ȡ��ѡ��nnadflashоƬ*/
	deselect_chip();
}

/*nand flash erase*/
int NF_Erase(unsigned long addr)
{
	int ret;

	/*ѡ��flashоƬ*/
	select_chip();

	/*���RnB*/
	clear_RnB();

	/*��������0x60*/
	send_cmd(0x60);

	/*�����е�ַ*/
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);

	/*��������D0*/
	send_cmd(0xD0);

	/*�ȴ�RnB*/
	wait_RnB();

	/*��������0x70*/
	send_cmd(0x70);

	/*��ȡ�������*/
	ret = NFDATA;

	/*ȡ��ѡ��flashоƬ*/
	deselect_chip();

	return (ret);
}

/*nand flash write*/
int NF_WritePage(unsigned long addr, unsigned char *buff)
{
	unsigned int i, ret = 0;

	/*ѡ��nandflash*/
	select_chip();

	/*���RnB*/
	clear_RnB();

	/*����0x80����*/
	send_cmd(0x80);

	/*����2���е�ַ*/
	send_addr(0x00);
	send_addr(0x00);

	/*����3���е�ַ*/
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)*0xff);

	/*��������*/
	for (i = 0; i < 2048; i++)
	{
		NFDATA = buff[i];
	}

	/*����0x10����*/
	send_cmd(0x10);

	/*�ȴ�RnB*/
	wait_RnB();

	/*����0x70����*/
	send_cmd(0x70);

	/*��ȡд����*/
	ret = NFDATA;     /*0д��ɹ���1д��ʧ��*/

	/*ȡ��ѡ��nandflash*/
	deselect_chip();

	return (ret);
}

/*��nand flash ��������ݶ�ȡ���ڴ�*/
void nand_to_ram(unsigned long start_addr, unsigned char *sdram_addr, int size)
{
	int i;
	
	/*��ҳ����û���е�ַ�����е�ַռ��ַ�ĵ�11λ����������11*/
	for (i = (start_addr >> 11); size > 0; )
	{
		NF_PageRead(i, sdram_addr);
		size -= 2048;                   /*һҳ2K byte*/
		sdram_addr += 2048;
		i++;
	}
}
