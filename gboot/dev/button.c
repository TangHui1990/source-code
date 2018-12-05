#define GPGCON (volatile unsigned long*)0x56000060

/*
 * K1,K2,K3,K4对应GPG0、GPG3、GPG5、GPG6
 */
 
#define GPG0_MSK (3 << (0*2))
#define GPG3_MSK (3 << (3*2))
#define GPG5_MSK (3 << (5*2))
#define GPG6_MSK (3 << (6*2))
 
#define GPG0_INIT (0x2 << (0*2))
#define GPG3_INIT (0x2 << (3*2))
#define GPG5_INIT (0x2 << (5*2))
#define GPG6_INIT (0x2 << (6*2))
 
void button_init(void)
{
	*(GPGCON) &= ~(GPG0_MSK | GPG3_MSK | GPG5_MSK | GPG6_MSK);   /*全部清零*/
	*(GPGCON) |= GPG0_INIT | GPG3_INIT |GPG5_INIT | GPG6_INIT; /*配置为EINT模式*/ 
}