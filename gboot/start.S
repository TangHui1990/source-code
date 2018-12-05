.text            @代码段
.global _start   @将_start声明为全局的 
_start:          @入口处
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq
	ldr pc, _reset
	
_undefined_instruction: .word undefined_instruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq
_reset: .word reset

undefined_instruction:    @指令异常
	nop

software_interrupt:       @软中断
	nop

prefetch_abort:           @预取指令异常
	nop

data_abort:		  @数据访问异常
	nop

not_used:                 @空位
	nop		  

irq:                      @一般类型的中断
	sub lr, lr, #4     @lr的位置在pc+8处，所以要指向下一条要执行的指令应该减4
	stmfd sp!, {r0-r12, lr} @保存环境，stmfd是批量处理指令，把r0到r12和lr的值全部保存到栈里
	bl handle_int           @跳转到中断处理函数处运行
	ldmfd sp!, {r0-r12, pc}^ @恢复环境，把r0到r12和lr的值从栈中恢复

fiq:                      @快速中断   
	nop

reset:                      @复位
	bl set_svc          @设置处理器为svc模式
	bl disable_watchdog @关闭看门狗
	bl disable_interrupt @关闭中断
	bl disable_mmu       @关闭MMU
	bl clock_init        @时钟初始化
	bl init_sdram        @内存初始化
	bl init_stack        @栈的初始化
	bl clean_bss         @初始化BSS段
	bl nandflash_init
	bl copy_to_ram	     @代码搬移
	ldr pc, =gboot_main  @一跃跳进C语言的大门
	@bl light_led         @点亮LED,LED是检验我们程序是否正确的依据，所以放最后
	
set_svc:                  @设置处理器为svc模式
	mrs r0, cpsr      @将cpsr里面的值复制到r0中
	bic r0, r0, #0x1f @将r0中的后5位清零
	orr r0, r0, #0xd3 @将处理器设置成svc模式，并disable IRQ,FIQ
	msr cpsr, r0      @将r0中的值写回cpsr
	mov pc, lr        @返回

#define pWTCON 0x53000000 @定义一个常量，pWTCON保存的是0x53000000
disable_watchdog:          
	ldr r0, =pWTCON   @使用伪指令将常量复制到r0
	mov r1, #0x0      @将0赋值到r1
	str r1, [r0]      @将r1的值写入到r0所表示的地址处
	mov pc, lr        @放回

disable_interrupt:
	MVN r1, #0x0      @将0x0取反，并赋值给r1
	ldr r0, =0x4A000008 @使用伪指令ldr将地址4A000008赋值给r0
	str r1, [r0]        @将r1的值写入r0处的地址 
	mov pc, lr          @放回
	
disable_mmu:
	mcr p15, 0, r0, c7, c7, 0 @是cache失效
	                          @MCR p15,0,Rd,c7,c7,0这里的Rd我们选择r0
	mrc p15, 0, r0, c1, c0, 0 @这个和cpsr一样的先必须读出来，改变，然后再写进去，这条语句是将寄存器的值读出
	bic r0, r0, #0x00000007   @将第0位，1位，2位清0
	mcr p15, 0, r0, c1, c0, 0 @将值写回去
	mov pc, lr                @放回

#define CLKDIVN 0x4C000014
#define MPLLCON 0x4c000004
#define MPLL_405MHZ (127<<12)|(2<<4)|(1<<0)
clock_init:
	@设置分频系数，FCLK:HCLK:PCLK = 1：4：8
	@分频系数不能随便设置，设置的不好可能对后面的程序有影响，1:4:8这个比例是参考uboot的
	ldr r0, =CLKDIVN    @将CLKDIVN寄存器的地址赋值给r0  
	mov r1, #0x05        @HDIVN=2，PDIVN=1所以CLKDIVN应设置为0x05
	str r1, [r0]	    @将0x05写入CLKDIVN寄存器的地址处
	@设置CPU为异步模式
	mrc p15, 0, r0, c1, c0, 0 @参考2440芯片手册，有范例
	orr r0, r0, #0xC0000000   @这个值的设置参考2440手册和arm核手册p15的第一个寄存器c1的30位，31位
	mcr p15, 0, r0, c1, c0, 0 @参考2440芯片手册，有范例
	@设置FCLK
	ldr r0, =MPLLCON
	ldr r1, =MPLL_405MHZ      @这个值参考2440芯片手册
	str r1, [r0]              @将值装载到MPLLCON寄存器地址处
	mov pc, lr                @返回

#define mem_contrl 0x48000000
init_sdram:
	ldr r0, =mem_contrl       @r0保存存储控制器的第一个地址
	@一共13个寄存器，每个寄存器4个字节
	add r3, r0, #4*13         @r3保存存储控制器的最后一个寄存器的地址
	adrl r1, mem_data         @把表的首地址存入r1，表也可以理解为数组
	
@实现循环
0:
	ldr r2, [r1], #4          @将数组的第一个值取出存入r2，然后r1+4
	str r2, [r0], #4          @将r2的值写入到第一个寄存器中，然后寄存器指针加4
	cmp r0, r3                @比较r0和r3
	bne 0b                    @如果不相等，跳转到0标号处继续执行，b表示向前跳

	mov pc, lr


@定义一个标号，指明一个地址，表示我们的数据放在这个地址处
@long是伪指令，指明数据的长度
@表也可以理解为数组
mem_data:
	.long 0x22000000
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00018001
	.long 0x00018001
	.long 0x008c04f5
	.long 0x000000b1
	.long 0x00000030
	.long 0x00000030

copy_to_ram:
        @ldr r0, =0x0          @起点
	@ldr r1, =0x30008000   @终点
	@add r3, r0, #1024*4   @大小
@循环copy
@copy_loop:
	@ldr r2, [r0], #4      @将源地址处的值写入r2，并把源地址加4     
	@str r2, [r1], #4      @将r2的值拷贝到目的地址处，并把目的地址加4
	@cmp r0, r3            @比较r0,r3
	@bne copy_loop         @如果不相等，继续循环
	@mov pc, lr

	mov r0, #0x00          @从nand flash的0地址处开始copy
	ldr r1, =_start        @启点        
	ldr r2, =bss_end       @终点
	sub r2, r2, r1         @大小
	mov ip, lr             @保存lr
	bl nand_to_ram         @跳转去执行c语言
	mov lr, ip             @恢复lr
	mov pc, lr             @返回


@栈的初始化非常简单只需要给sp指针符一个地址,这里栈的大小设置为64M即内存地址开始处加64M（0x30000000+64M）
init_stack:
	@初始化不同系统模式下的栈
	msr cpsr_c, #0xd2
        ldr sp, =0x33000000 //初始化r13_irq
        msr cpsr_c, #0xd3
	ldr sp, =0x34000000   //初始化r13_svc
	mov pc ,lr

@初始化BSS段
clean_bss:
	ldr r0, =bss_start    @bss的起始地址赋值给r0
	ldr r1, =bss_end      @bss的结束地址赋值给r1
	cmp r0, r1            @比较起始地址和结束地址是否相等，如果相等就不用初始化了，直接返回
	moveq pc, lr          @相等返回

clean_loop:
	mov r2, #0            @如果起始地址和结束地址不相等，把0赋值给r2
	str r2, [r0], #4      @把0写进bss段的起始地址并把地址加4
	cmp r0, r1            @比较起始地址和结束地址是否相等
	bne clean_loop        @不相等，继续循环
	mov pc, lr

#define GPBCON 0x56000010         @定义宏
#define GPBDAT 0x56000014         
light_led:
	@配置LED的控制寄存器使其为输出状态
	ldr r0, =GPBCON                     @使用伪指令将GPBCON的地址赋值给r0寄存器
	ldr r1, =0x15400         @使用伪指令将GPBCON要设置的值赋值给r1寄存器
	str r1, [r0]             @将r1的值写入r0，也就完成了将LED的引脚设置成输出的任务
	@设置LED的数据寄存器，点亮LED
	ldr r0, =GPBDAT
	ldr r1, =0x6BF           @GPBDAT寄存器的每一位对应一个引脚        
	str r1, [r0]
	mov pc, lr





	

                                  
