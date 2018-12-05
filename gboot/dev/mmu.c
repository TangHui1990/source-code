/* 
 * 用于段描述符的一些宏定义
 */ 
#define MMU_FULL_ACCESS    (3 << 10) /*访问权限*/
#define MMU_DOMAIN         (0 << 5)  /*属于哪个域*/
#define MMU_SPECIAL        (1 << 4)  /*必须是1*/
#define MMU_CACHEABLE      (1 << 3)  /*cacheable*/
#define MMU_BUFFERABLE     (1 << 2)  /*bufferable*/
#define MMU_SECTION        (2)       /*表示这是段描述符*/
#define MMU_SECDESC         (MMU_FULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_SECTION)
#define MMU_SECDESC_WB      (MMU_FULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_CACHEABLE | MMU_BUFFERABLE | MMU_SECTION)
 
/*建立页表*/
void creat_page_table(void)
{
	unsigned long *ttb = (unsigned long *)0x30000000; /*页的起始地址*/
	
	unsigned long vaddr, paddr;
	
	vaddr = 0xA0000000; /*虚拟地址*/
	paddr = 0x56000000; /*物理基地址，GPBDAT的地址为0x56000014, 14为偏移量*/
	
	*(ttb + (vaddr >> 20)) = (paddr & 0xFFF00000) | MMU_SECDESC; 
	
	/*不能只映射LED的GPIO，要映射内存，因为我们要运行其他的程序，其他的程序是运行在内存里面的
	并且虚拟地址和物理地址起点（基地址）相同*/
	vaddr = 0x30000000;
	paddr = 0x30000000;
	while ( vaddr < 0x34000000 )
	{
		*(ttb + (vaddr >> 20)) = (paddr & 0xFFF00000) | MMU_SECDESC_WB;
		vaddr += 0x100000; /*1M*/
		paddr += 0x100000;
	}
}

void mmu_enable(void)
{
	__asm__(
		/*设置TTB*/
		"ldr r0, =0x30000000\n"
		"mcr p15, 0, r0, c2, c0, 0\n"
		
		/*不进行权限检查*/
		"mvn r0, #0\n"
		"mcr p15, 0, r0, c3, c0, 0\n"
		
		/*使能MMU*/
		"mrc p15, 0, r0, c1, c0, 0\n"
		"orr r0, r0, #0x0001\n"
		"mcr p15, 0, r0, c1, c0, 0\n"
		:
		:		
		);
}

void mmu_init(void)
{
	creat_page_table();

	mmu_enable();
}