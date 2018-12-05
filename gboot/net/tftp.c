#include "arp.h"
#include "string.h"

#define SDRAM_KERNEL_START 0x31000000

/*ת���������ֽ���(���)*/
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

/*��tftp����������tftp�����*/
void tftp_send_request(const char *filename)
{
	/*ΪIP��UDP�ײ�Ԥ���ռ�*/
	u8* ptftp = &sendbuf[200];
	u32 tftp_len = 0;
	UDP_HDR *udphdr;
	u8 *iphdr;

	/*�������˴洢*/
	ptftp[0] = 0x00;
	ptftp[1] = 0x01;
	tftp_len += 2;

	/*�ļ���*/
	sprintf(&ptftp[tftp_len], "%s", filename);
	tftp_len += strlen(filename);

	ptftp[tftp_len] = '\0';
	tftp_len += 1;

	/*ģʽ(�˽���)*/
	sprintf(&ptftp[tftp_len], "%s", "octet");
	tftp_len += strlen("octet");

	ptftp[tftp_len] = '\0';
	tftp_len += 1;

	/*����UDP�ײ���IP�ײ�*/
	
	/*ָ���Ƶ�UDP�ײ���*/
	udphdr = ptftp - sizeof(UDP_HDR);
	/*UDP�ײ�����IP�ײ�����IP�ײ���װ��̫���ײ�*/
	/*ָ���Ƶ�IP�ײ�����̫���ײ���*/
	iphdr = ptftp - sizeof(UDP_HDR) + sizeof(ETH_HDR);

	/*UDPͷ����Ϣ*/
	/*Դ�˿�*/
	udphdr->sport = HON(48915);
	/*Ŀ�Ķ˿ڱ�����69*/
	udphdr->dport = HON(69);
	/*����(��ȥ�²�Э��)*/
	udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
	udphdr->udpchksum = 0x00;

	/*����IP֡ͷ��Ϣ*/
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	/*����(��ȥ�²�Э��)*/
	udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0x00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 17;

	memcpy(udphdr->iphdr.srcipaddr, ip_addr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);

	udphdr->iphdr.ipchksum = 0;
	udphdr->iphdr.ipchksum = checksum(iphdr, 20);
	
	/*������̫��ͷ��Ϣ*/
	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.type = HON(PROTO_IP);

	dm9000_tx((u32*)udphdr, sizeof(UDP_HDR)+tftp_len);
}


/*�յ�tftp���������͹����������Ժ󣬸�ack*/
void tftp_send_ack(u16 blocknum)
{
	u8 *ptftp = &sendbuf[200];
	u32 tftp_len = 0;
	UDP_HDR *udphdr;
	u8 *iphdr;

	/*��˴洢*/
	ptftp[0] = 0x00;
	ptftp[1] = 0x04;
	tftp_len += 2;

	ptftp[2] = ((blocknum&0xff00)>>8);
	ptftp[3] = (blocknum&0xff);
	tftp_len += 2;

	udphdr = ptftp-sizeof(UDP_HDR);
	iphdr = ptftp-sizeof(UDP_HDR) + sizeof(ETH_HDR);


	/*UDPͷ����Ϣ*/
	/*Դ�˿�*/
	udphdr->sport = HON(48915);
	/*Ŀ�Ķ˿ڱ�����69*/
	udphdr->dport = HON(serverport);
	/*����(��ȥ�²�Э��)*/
	udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
	udphdr->udpchksum = 0x00;

	/*����IP֡ͷ��Ϣ*/
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	/*����(��ȥ�²�Э��)*/
	udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0x00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 17;

	memcpy(udphdr->iphdr.srcipaddr, ip_addr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);

	udphdr->iphdr.ipchksum = 0;
	udphdr->iphdr.ipchksum = checksum(iphdr, 20);
	
	/*������̫��ͷ��Ϣ*/
	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.type = HON(PROTO_IP);

	dm9000_tx((u32*)udphdr, sizeof(UDP_HDR)+tftp_len);
}

/*�������յ���tftp���ݰ�*/
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
			/*һ��tftp�������ݳ���Ϊ512�ֽ�*/
			tftp_down_addr = (u8*)(SDRAM_KERNEL_START+(curblock-1)*512);

			/*��ȥ������Ϳ��ŵ�4���ֽ�*/
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



















