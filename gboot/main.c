int gboot_main()
{
	unsigned int num;

#ifdef MMU_ON
	mmu_init();
#endif

	led_init();

	led_off();

	button_init();

	init_irq();

	uart_init();

//	dma_init();
//	dma_start();
	//lcd_init();
	//Lcd_Test();
	//
//	ts_init();
	dm9000_init();



	while ( 1 )
	{
		printf("\n*********<angelo_666@163.com>***********\n\r");
		printf("****************GBOOT*******************\n\r");
		printf("1:Download Linux Kernel from TFTP Server!\n\r");
		printf("2:Boot Linux from RAM!\n\r");
		printf("3:Send ARP to PC!\n\r");
		printf("\n\rPlease Select:");

		scanf("%d", &num);

		switch ( num )
		{
		case 1:
			tftp_send_request("zImage");
			break;

		case 2:
			boot_linux();
			break;

		case 3:
			arp_request();
			break;

		default:
			break;
		}
	}

    return (0);
}
