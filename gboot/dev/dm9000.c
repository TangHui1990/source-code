/*参考uboot/driver/net下的dm9000驱动*/

#include "dm9000.h"
#include "arp.h"

/*以太网包最大的长度*/
#define PTK_MAX_LEN 1522

u8 host_mac_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8 mac_addr[6] = {9, 8, 7, 6, 5, 4};
u8 ip_addr[4] = {192, 168, 1, 30};
u8 host_ip_addr[4] = {192, 168, 1, 100};

u8 *buffer = &arpbuf;

/***********DM9000 index ande data address*************/
#define DM_ADD (*((volatile unsigned short*)0x20000300))
#define DM_DAT (*((volatile unsigned short*)0x20000304))
/**************************************************/

/***********片选寄存**************************/
#define BWSCON (*((volatile unsigned long*)0x48000000))
#define BANKCON4 (*((volatile unsigned long*)0x48000014))
/**************************************************/

/******************interrupt*************************/
#define GPFCON (*((volatile unsigned long*)0x56000050))
#define EXTINT0 (*((volatile unsigned long*)0x56000088))
#define INTMSK (*((volatile unsigned long*)0x4A000008))
#define EINTMASK (*((volatile unsigned long*)0x560000a4))
#define SRCPND (*((volatile unsigned long*)0x4A000000))
#define INTSUBMSK (*((volatile unsigned long*)0x4A00001C))
#define INTPND (*((volatile unsigned long*)0x4A000010))
#define EINTPEND  (*(volatile unsigned *)0x560000a8)	//External interrupt pending

/**************************************************/

/*参考U-Boot\arch\arm\cpu\arm920t\s3c2440\lowlevel_init.S*/
#define B4_Tacs			0x0	/*  0clk */
#define B4_Tcos			0x0	/*  3clk */
#define B4_Tacc			0x7	/* 14clk */
#define B4_Tcoh			0x1	/*  1clk */
#define B4_Tah			0x0	/*  0clk */
#define B4_Tacp			0x0  /*  6clk */
#define B4_PMC			0x0	/* normal */

/*设置片选*/
void cs_init(void)
{
	BWSCON &= ~(0x3 << 16);
	BWSCON |= (0x01 << 16);

	BANKCON4 = (B4_Tacs<<13)|(B4_Tcos<<11)|(B4_Tacc<<8)|(B4_Tcoh<<6)\
		|(B4_Tah<<4)|(B4_Tacp<<2)|(B4_PMC<<0);
	
}

/*中断初始化*/
void int_init(void)
{
	GPFCON &= ~(0x3 << 14);
	GPFCON |= (0X2 << 14);

	/*外部中断，高电平触发中断*/
	EXTINT0 &= ~(0x7 << 28);
	EXTINT0 |= (0x1 << 28);

	EINTMASK &= ~(0x1 << 7); //充许外部中断

	INTMSK &= ~(1 << 4);
	//INTSUBMSK &= ~(1 << 7);

	SRCPND |= (1 << 4);
	INTPND |= (1 << 4);
}

/*写寄存器*/
void dm9000_reg_write(u16 reg, u16 data)
{
	DM_ADD = reg;
	DM_DAT = data;
}

/*读寄存器*/
u8 dm9000_reg_read(u16 reg)
{
	DM_ADD = reg;

	return (DM_DAT);
}

void dm9000_reset(void)
{
	//DM9000_DBG("resetting DM9000\n");

	/* Reset DM9000,
	   see DM9000 Application Notes V1.22 Jun 11, 2004 page 29 */

	/* DEBUG: Make all GPIO0 outputs, all others inputs */
	dm9000_reg_write(DM9000_GPCR, GPCR_GPIO0_OUT);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	
	dm9000_reg_write(DM9000_GPCR, GPCR_GPIO0_OUT);
	dm9000_reg_write(DM9000_GPR, 0);
	/* Step 2: Software reset */
	
	dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));

	//do {
		//DM9000_DBG("resetting the DM9000, 1st reset\n");
		//udelay(25); /* Wait at least 20 us */
	//} while (dm9000_reg_read(DM9000_NCR) & 1);

	dm9000_reg_write(DM9000_NCR, 0);
	dm9000_reg_write(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST)); /* Issue a second reset */
	dm9000_reg_write(DM9000_NCR, 0);

	//do {
		//DM9000_DBG("resetting the DM9000, 2nd reset\n");
		//udelay(25); /* Wait at least 20 us */
	//} while (dm9000_reg_read(DM9000_NCR) & 1);

	/* Check whether the ethernet controller is present */
	//if ((dm9000_reg_read(DM9000_PIDL) != 0x0) ||
	   // (dm9000_reg_read(DM9000_PIDH) != 0x90))
		//printf("ERROR: resetting DM9000 -> not responding\n");
}

/*捕获dm9000网卡*/
void dm9000_probe(void)
{
	u32 id_val;
	
	id_val = dm9000_reg_read(DM9000_VIDL);
	id_val |= dm9000_reg_read(DM9000_VIDH) << 8;
	id_val |= dm9000_reg_read(DM9000_PIDL) << 16;
	id_val |= dm9000_reg_read(DM9000_PIDH) << 24;
	if (id_val == DM9000_ID) 
	{
		printf("Found DM9000 Device!!!\n\r");
	} 
	else 
	{
		printf("Not Found DM9000 Device!!!\n\r");
	}
}


/*网卡初始化函数*/
void dm9000_init(void)
{
	u32 i;

	/*设置片选*/
	cs_init();

	/*中断初始化*/
	int_init();

	/*复位设备*/
	dm9000_reset();

	/*捕获dm9000网卡*/
	dm9000_probe();

	/*MAC地址初始化*/
	/* Program operating register, only internal phy supported */
	dm9000_reg_write(DM9000_NCR, 0x0);
	/* TX Polling clear */
	dm9000_reg_write(DM9000_TCR, 0);
	/* Less 3Kb, 200us */
	dm9000_reg_write(DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);
	/* Flow Control : High/Low Water */
	dm9000_reg_write(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));
	/* SH FIXME: This looks strange! Flow Control */
	dm9000_reg_write(DM9000_FCR, 0x0);
	/* Special Mode */
	dm9000_reg_write(DM9000_SMCR, 0);
	/* clear TX status */
	dm9000_reg_write(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	/* Clear interrupt status */
	dm9000_reg_write(DM9000_ISR, ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS);
	
	/* 填充MAC地址 */
	for (i = 0; i < 6; i++)
	{
		dm9000_reg_write(DM9000_PAR+i, mac_addr[i]);
	}

	/* 激活DM9000网卡 */
	/* Activate DM9000 */
	/* RX enable */
	dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	dm9000_reg_write(DM9000_IMR, IMR_PAR);
}

/*网卡发送函数*/
void dm9000_tx(u8 *data, u32 length)
{
	u32 i;
	
	/*禁止中断*/
	dm9000_reg_write(DM9000_IMR,0x80);

	/* 写入发送数据的长度 */
	dm9000_reg_write(DM9000_TXPLL, length & 0xff);
	dm9000_reg_write(DM9000_TXPLH, (length >> 8) & 0xff);

	/*写入待发送的数据*/
	/* Move data to DM9000 TX RAM */
       //DM9000_outb(DM9000_MWCMD, DM9000_IO); /* Prepare for TX-data */
       DM_ADD = DM9000_MWCMD;

	for (i = 0; i < length; i+=2)
	{
		DM_DAT = data[i] | (data[i+1] << 8);
	}
	
	/*启动发送*/
	dm9000_reg_write(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

	/*等待发送结束*/
	while ( 1 )
	{
		u8 status;
		status = dm9000_reg_read(DM9000_TCR);

		if ((status & 0x01) == 0x00)
		{
			break;
		}
	}
	
	/* 清除发送标志 */
	dm9000_reg_write(DM9000_NSR, 0x2c); 
	
	/*恢复中断使能*/
	dm9000_reg_write(DM9000_IMR,0x81);
}

/*网卡接收函数*/
int dm9000_rx(u8 *data)
{
	u16 status, len;
	u16 tmp;
	u32 i;
	u8 ready = 0;
	
	/*判断是否产生中断且清除*/
	if (dm9000_reg_read(DM9000_ISR) & 0x01)
	{
		dm9000_reg_write(DM9000_ISR, 0x01); //写1清除
	}
	else
	{
		return (0);
	}

	/*空读*/
	ready = dm9000_reg_read(DM9000_MRCMDX);
    
    if ((ready & 0x01) != 0x01)
    {
    	ready = dm9000_reg_read(DM9000_MRCMDX);
    	if ((ready & 0x01) != 0x01)
		{
			return 0;
		}	   
    }

	/*读取状态*/
	status = dm9000_reg_read(DM9000_MRCMD);

	/*读取包的长度*/
	len = DM_DAT;

	/*读取包数据*/
	if (len < PTK_MAX_LEN)
	{
		for (i=0; i<len; i+=2)
		{
			tmp = DM_DAT;
			data[i] = tmp & 0x0ff;
			data[i+1] = (tmp>>8)&0x0ff;
		}
	}

	return (len);
}

void int_issue(void)
{
	packet_len = dm9000_rx(buffer);

	/*处理网卡收到的包*/
	net_handle(buffer, packet_len);
	//arp_process();

	SRCPND = (1<<4);
       INTPND = (1<<4);
       EINTPEND |= 1<<7;
}

void dm9000_arp(void)
{
	while ( 1 )
	{
		arp_request();
	}
}




