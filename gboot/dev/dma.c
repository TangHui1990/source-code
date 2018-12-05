#define DISRC0        (*(volatile unsigned long*)0x4B000000)
#define DISRCC0       (*(volatile unsigned long*)0x4B000004)
#define DIDST0        (*(volatile unsigned long*)0x4B000008)
#define DIDSTC0       (*(volatile unsigned long*)0x4B00000C)
#define DCON0         (*(volatile unsigned long*)0x4B000010)
#define DMASKTRIG0    (*(volatile unsigned long*)0x4B000020)

#define UTXH0         (volatile unsigned long*)0x50000020

const char *buf = "Hello The world!";

/*DMA初始化*/
void dma_init(void)
{

	/*源地址*/
	DISRC0  = (unsigned int)buf;
	DISRCC0 = (0 << 0) | (0 << 1); /*源控制寄存器*/

	/*目的地址*/
     DIDST0 = UTXH0;
	 DIDSTC0 = (1 << 0) | (1 << 1) | (0 << 2); /*目的控制寄存器*/

	/*配置控制寄存器*/
	DCON0 |= (0x1 << 24) | (1 << 23) | (1 << 22) | (16 << 0); 
}

/*开启DMA*/
void dma_start(void)
{
	DMASKTRIG0 |= (1 << 1);
}
