#define DISRC0        (*(volatile unsigned long*)0x4B000000)
#define DISRCC0       (*(volatile unsigned long*)0x4B000004)
#define DIDST0        (*(volatile unsigned long*)0x4B000008)
#define DIDSTC0       (*(volatile unsigned long*)0x4B00000C)
#define DCON0         (*(volatile unsigned long*)0x4B000010)
#define DMASKTRIG0    (*(volatile unsigned long*)0x4B000020)

#define UTXH0         (volatile unsigned long*)0x50000020

const char *buf = "Hello The world!";

/*DMA��ʼ��*/
void dma_init(void)
{

	/*Դ��ַ*/
	DISRC0  = (unsigned int)buf;
	DISRCC0 = (0 << 0) | (0 << 1); /*Դ���ƼĴ���*/

	/*Ŀ�ĵ�ַ*/
     DIDST0 = UTXH0;
	 DIDSTC0 = (1 << 0) | (1 << 1) | (0 << 2); /*Ŀ�Ŀ��ƼĴ���*/

	/*���ÿ��ƼĴ���*/
	DCON0 |= (0x1 << 24) | (1 << 23) | (1 << 22) | (16 << 0); 
}

/*����DMA*/
void dma_start(void)
{
	DMASKTRIG0 |= (1 << 1);
}
