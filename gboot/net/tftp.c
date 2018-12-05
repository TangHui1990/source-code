#include "arp.h"
#include "string.h"

#define SDRAM_KERNEL_START 0x31000000

/*转换成网络字节序(大端)*/
#define HON(n) ((((u16)(n & 0xff))<<8) | (((u16)(n & 0xff00))>>8))

u8 sendbuf[1024];
u8* tftp_down_addr;
u16 serverport = 0;
u16 curblock = 1;

u16 checksum(u8 *ptr, int len)
{   
	u32 sum = 0;    
	u16 *p = (u16 *)ptr;    

	while (len > 1)   
	{        
		sum += *p++;        
		len -= 2;    
	}       

	if(len == 1)        
	sum += *(u8 *)p;        

	while(sum>>16)        
	sum = (sum&0xffff) + (sum>>16);        

	return (u16)((~sum)&0xffff);
}

/*给tftp服务器发送tftp请求包*/
void tftp_send_request(const char *filename)
{
	/*为IP和UDP首部预留空间*/
	u8* ptftp = &sendbuf[200];
	u32 tftp_len = 0;
	UDP_HDR *udphdr;
	u8 *iphdr;

	/*操作码大端存储*/
	ptftp[0] = 0x00;
	ptftp[1] = 0x01;
	tftp_len += 2;

	/*文件名*/
	sprintf(&ptftp[tftp_len], "%s", filename);
	tftp_len += strlen(filename);

	ptftp[tftp_len] = '\0';
	tftp_len += 1;

	/*模式(八进制)*/
	sprintf(&ptftp[tftp_len], "%s", "octet");
	tftp_len += strlen("octet");

	ptftp[tftp_len] = '\0';
	tftp_len += 1;

	/*设置UDP首部和IP首部*/
	
	/*指针移到UDP首部处*/
	udphdr = ptftp - sizeof(UDP_HDR);
	/*UDP首部包含IP首部，而IP首部封装以太网首部*/
	/*指针移到IP首部的以太网首部后*/
	iphdr = ptftp - sizeof(UDP_HDR) + sizeof(ETH_HDR);

	/*UDP头部信息*/
	/*源端口*/
	udphdr->sport = HON(48915);
	/*目的端口必须是69*/
	udphdr->dport = HON(69);
	/*长度(减去下层协议)*/
	udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
	udphdr->udpchksum = 0x00;

	/*设置IP帧头信息*/
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	/*长度(减去下层协议)*/
	udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0x00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 17;

	memcpy(udphdr->iphdr.srcipaddr, ip_addr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);

	udphdr->iphdr.ipchksum = 0;
	udphdr->iphdr.ipchksum = checksum(iphdr, 20);
	
	/*设置以太网头信息*/
	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.type = HON(PROTO_IP);

	dm9000_tx((u32*)udphdr, sizeof(UDP_HDR)+tftp_len);
}


/*收到tftp服务器发送过来的数据以后，给ack*/
void tftp_send_ack(u16 blocknum)
{
	u8 *ptftp = &sendbuf[200];
	u32 tftp_len = 0;
	UDP_HDR *udphdr;
	u8 *iphdr;

	/*大端存储*/
	ptftp[0] = 0x00;
	ptftp[1] = 0x04;
	tftp_len += 2;

	ptftp[2] = ((blocknum&0xff00)>>8);
	ptftp[3] = (blocknum&0xff);
	tftp_len += 2;

	udphdr = ptftp-sizeof(UDP_HDR);
	iphdr = ptftp-sizeof(UDP_HDR) + sizeof(ETH_HDR);


	/*UDP头部信息*/
	/*源端口*/
	udphdr->sport = HON(48915);
	/*目的端口必须是69*/
	udphdr->dport = HON(serverport);
	/*长度(减去下层协议)*/
	udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
	udphdr->udpchksum = 0x00;

	/*设置IP帧头信息*/
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	/*长度(减去下层协议)*/
	udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0x00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 17;

	memcpy(udphdr->iphdr.srcipaddr, ip_addr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);

	udphdr->iphdr.ipchksum = 0;
	udphdr->iphdr.ipchksum = checksum(iphdr, 20);
	
	/*设置以太网头信息*/
	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.type = HON(PROTO_IP);

	dm9000_tx((u32*)udphdr, sizeof(UDP_HDR)+tftp_len);
}

/*出来接收到的tftp数据包*/
void tftp_process(u8 *buf, u32 len, u16 port)
{
	u32 i;
	u32 tftp_len;

	serverport = port;

	TFTP_PAK *ptftp = buf + sizeof(UDP_HDR);

	tftp_len = len - sizeof(UDP_HDR);

	if (HON(ptftp->opcode) == 3)
	{
		if (HON(ptftp->blocknum)  == curblock)
		{
			/*一个tftp包的数据长度为512字节*/
			tftp_down_addr = (u8*)(SDRAM_KERNEL_START+(curblock-1)*512);

			/*减去操作码和块编号的4个字节*/
			for (i = 0; i <(tftp_len-4); i++)
			{
				*(tftp_down_addr++) = *(ptftp->data+i);
			}

			tftp_send_ack(HON(ptftp->blocknum));

			curblock++;

			if ( (ptftp->blocknum % 10) == 0)
			{
				printf(".");
			}

			if ((tftp_len-4) < 512)
			{
				curblock = 1;
				printf("\n\r");
				printf("\n\r1:Download Linux Kernel from TFTP Server!\n\r");
				printf("2:Boot Linux from RAM!\n\r");
				printf("3:Send ARP to PC!\n\r");
				printf("\n\rPlease Select:");
			}
		}
	}
}



















