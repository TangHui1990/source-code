/*�ο�uboot/driver/net�µ�dm9000����*/

#include "dm9000.h"
#include "arp.h"

/*��̫�������ĳ���*/
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

/***********Ƭѡ�Ĵ�**************************/
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

/*�ο�U-Boot\arch\arm\cpu\arm920t\s3c2440\lowlevel_init.S*/
#define B4_Tacs			0x0	/*  0clk */
#define B4_Tcos			0x0	/*  3clk */
#define B4_Tacc			0x7	/* 14clk */
#define B4_Tcoh			0x1	/*  1clk */
#define B4_Tah			0x0	/*  0clk */
#define B4_Tacp			0x0  /*  6clk */
#define B4_PMC			0x0	/* normal */

/*����Ƭѡ*/
void cs_init(void)
{
	BWSCON &= ~(0x3 << 16);
	BWSCON |= (0x01 << 16);

	BANKCON4 = (B4_Tacs<<13)|(B4_Tcos<<11)|(B4_Tacc<<8)|(B4_Tcoh<<6)\
		|(B4_Tah<<4)|(B4_Tacp<<2)|(B4_PMC<<0);
	
}

/*�жϳ�ʼ��*/
void int_init(void)
{
	GPFCON &= ~(0x3 << 14);
	GPFCON |= (0X2 << 14);

	/*�ⲿ�жϣ��ߵ�ƽ�����ж�*/
	EXTINT0 &= ~(0x7 << 28);
	EXTINT0 |= (0x1 << 28);

	EINTMASK &= ~(0x1 << 7); //�����ⲿ�ж�

	INTMSK &= ~(1 << 4);
	//INTSUBMSK &= ~(1 << 7);

	SRCPND |= (1 << 4);
	INTPND |= (1 << 4);
}

/*д�Ĵ���*/
void dm9000_reg_write(u16 reg, u16 data)
{
	DM_ADD = reg;
	DM_DAT = data;
}

/*���Ĵ���*/
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

/*����dm9000����*/
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


/*������ʼ������*/
void dm9000_init(void)
{
	u32 i;

	/*����Ƭѡ*/
	cs_init();

	/*�жϳ�ʼ��*/
	int_init();

	/*��λ�豸*/
	dm9000_reset();

	/*����dm9000����*/
	dm9000_probe();

	/*MAC��ַ��ʼ��*/
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
	
	/* ���MAC��ַ */
	for (i = 0; i < 6; i++)
	{
		dm9000_reg_write(DM9000_PAR+i, mac_addr[i]);
	}

	/* ����DM9000���� */
	/* Activate DM9000 */
	/* RX enable */
	dm9000_reg_write(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	dm9000_reg_write(DM9000_IMR, IMR_PAR);
}

/*�������ͺ���*/
void dm9000_tx(u8 *data, u32 length)
{
	u32 i;
	
	/*��ֹ�ж�*/
	dm9000_reg_write(DM9000_IMR,0x80);

	/* д�뷢�����ݵĳ��� */
	dm9000_reg_write(DM9000_TXPLL, length & 0xff);
	dm9000_reg_write(DM9000_TXPLH, (length >> 8) & 0xff);

	/*д������͵�����*/
	/* Move data to DM9000 TX RAM */
       //DM9000_outb(DM9000_MWCMD, DM9000_IO); /* Prepare for TX-data */
       DM_ADD = DM9000_MWCMD;

	for (i = 0; i < length; i+=2)
	{
		DM_DAT = data[i] | (data[i+1] << 8);
	}
	
	/*��������*/
	dm9000_reg_write(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

	/*�ȴ����ͽ���*/
	while ( 1 )
	{
		u8 status;
		status = dm9000_reg_read(DM9000_TCR);

		if ((status & 0x01) == 0x00)
		{
			break;
		}
	}
	
	/* ������ͱ�־ */
	dm9000_reg_write(DM9000_NSR, 0x2c); 
	
	/*�ָ��ж�ʹ��*/
	dm9000_reg_write(DM9000_IMR,0x81);
}

/*�������պ���*/
int dm9000_rx(u8 *data)
{
	u16 status, len;
	u16 tmp;
	u32 i;
	u8 ready = 0;
	
	/*�ж��Ƿ�����ж������*/
	if (dm9000_reg_read(DM9000_ISR) & 0x01)
	{
		dm9000_reg_write(DM9000_ISR, 0x01); //д1���
	}
	else
	{
		return (0);
	}

	/*�ն�*/
	ready = dm9000_reg_read(DM9000_MRCMDX);
    
    if ((ready & 0x01) != 0x01)
    {
    	ready = dm9000_reg_read(DM9000_MRCMDX);
    	if ((ready & 0x01) != 0x01)
		{
			return 0;
		}	   
    }

	/*��ȡ״̬*/
	status = dm9000_reg_read(DM9000_MRCMD);

	/*��ȡ���ĳ���*/
	len = DM_DAT;

	/*��ȡ������*/
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

	/*���������յ��İ�*/
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



