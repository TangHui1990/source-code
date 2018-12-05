//http://www.114la.com/other/rgb.htm  RGB��ɫ��ѯ���ձ�

#define GPCCON	(*(volatile unsigned long*)0x56000020)
#define GPDCON	(*(volatile unsigned long*)0x56000030)
#define LCDCON1 (*(volatile unsigned long*)0x4D000000)
#define	LCDCON2 (*(volatile unsigned long*)0x4D000004)
#define LCDCON3 (*(volatile unsigned long*)0x4D000008)
#define LCDCON4 (*(volatile unsigned long*)0x4D00000C)
#define LCDCON5 (*(volatile unsigned long*)0x4D000010)
#define LCDSADDR1 (*(volatile unsigned long*)0x4D000014)
#define LCDSADDR2 (*(volatile unsigned long*)0x4D000018)
#define LCDSADDR3 (*(volatile unsigned long*)0x4D00001C)

#define GPGCON (*(volatile unsigned long*)0x56000060)

/*��ʱ��ɫ��*/
#define TPAL (*(volatile unsigned long*)0x4D000050)

/*�ο�2440�ֲ�LCDоƬ�ֲ�*/
#define VSPW    0
#define VPBD    17 
#define LINEVAL 271 
#define VFPD    3
#define CLKVAL  4      //TFT: 10 = 100 / [(CLKVAL+1) x 2]
#define HSPW    0
#define HBPD    37
#define HOZVAL  479
#define HFPD    1
/*
#define VSPW 9
#define VPBD 1
#define LINEVAL 271
#define VFPD 1
#define CLKVAL 4   //10 = 100/((CLKVAL+1)*2)
#define HSPW 40
#define HBPD 1
#define HOZVAL 479
#define HFPD 1*/

unsigned short LCDBUFFER[272][480];

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/*gpio����*/
void lcd_port_init(void)
{
	GPCCON |= 0xaaaaaaaa;
	GPDCON |= 0xaaaaaaaa;
}

/*LCD����������*/
void lcd_control_init(void)
{
	LCDCON1 |= (CLKVAL<<8)|(0x3<<5)|(0xc<<1)|(0<<0);
	LCDCON2 = (VSPW<<0)|(VFPD<<6)|(LINEVAL<<14)|(VPBD<<24);
	LCDCON3 = (HFPD<<0)|(HOZVAL<<8)|(HBPD<<19);
	LCDCON4 |= (HSPW << 0);
	LCDCON5 = (1 << 11) | (1 << 8) | (1 << 9);

	LCDSADDR1 = ((((u32)LCDBUFFER>>1)&0x1fffff)) | (((u32)LCDBUFFER>>22)<<21);
	/*frame buffer������ַ��[21:1]λ*/
	LCDSADDR2 = (((u32)LCDBUFFER + 272*480*2) >> 1)&0x1fffff; 
	LCDSADDR3 = (0x0 << 11) | (480*2/2);

	TPAL = 0;
}

void lcd_init(void)
{
	lcd_port_init();
	lcd_control_init();

	/*��LCD��Դ*/
	GPGCON |= (0x3 << 8);
	LCDCON5 |= (1 << 3);
	LCDCON1 |= (1 << 0);
}

/*��ʾһ��ɫ�أ�Ҳ�Ǿ�����ʾһ���㣬�������������������*/
void Pait_point(unsigned int x, unsigned int y, unsigned int color)
{
	unsigned int red, green, blue;

	red = (color >> 19) & 0x1f;
	green = (color >> 10) & 0x3f;
	blue = (color >> 3) & 0x1f;

	LCDBUFFER[y][x] = (unsigned short)((red<<11)|(green<<5)|(blue));
}

/*ʹ����ʱ��ɫ����LCD��������ʹ����ʾһ����ɫ*/
void clearSrc(unsigned int color)
{
	TPAL = (1 << 24) | (color&0xffffff);
}

extern const unsigned char bmp_code[90200];

/*��ʾһ��ͼ��*/
/*x0,y0��LCD�����꣬Ҳ����ͼ����ʾ��LCD��ʲôλ��*/
/*wide,high��ͼ��Ŀ�Ⱥ͸߶�*/
/*bmp��ͼ��*/
void Paint_Bmp(u16 x0, u16 y0, u16 wide, u16 high, const u8 *bmp)
{
	u16 x, y;
	u16 c;
	u32 p = 0;

	for (y = y0; y < y0+high; y++)
	{
		for (x = x0; x < x0+wide; x++)
		{
			c = bmp[p] | (bmp[p+1]<<8); //ע��ϵͳ�Ĵ�С��

			if ((x < 480) && (y < 272))
			{
				LCDBUFFER[y][x] = c;
			}

			p = p + 2;
		}
	}
}

/*����LCD*/
void Lcd_Test(void)
{
	int x;

	for (x = 0; x<480; x++)
	{
		Pait_point(x++, 150, 0xff0000);
	}

	Paint_Bmp(0, 0, 220, 220, bmp_code);

	clearSrc(0xff0000);
}









