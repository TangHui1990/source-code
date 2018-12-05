#include "arp.h"

/*ת���������ֽ���(���)*/
#define HON(n) ((((u16)(n & 0xff))<<8) | (((u16)(n & 0xff00))>>8))

/*1.����ARP�����*/
void arp_request(void)
{
	/*����ARP�����*/
	memcpy(arpbuf.ethhdr.d_mac, host_mac_addr, 6);
	memcpy(arpbuf.ethhdr.s_mac, mac_addr, 6);
	arpbuf.ethhdr.type = HON(0x0806);    /*0x0806����ʾARP����*/
	arpbuf.hwtype =HON(1);
	arpbuf.protocol = HON(0x0800);
	arpbuf.hwlen = 6;
	arpbuf.protolen = 4;
	arpbuf.opcode = HON(1);
	memcpy(arpbuf.smac, mac_addr, 6);
	memcpy(arpbuf.sipaddr, ip_addr, 4);
	memcpy(arpbuf.dipaddr, host_ip_addr, 4);

	packet_len = 14+28;

	/*����DM9000�������ͺ������������*/
	dm9000_tx(&arpbuf, packet_len);
}



/*2.����ARPӦ���,��ȡMAC��ַ*/
u8 arp_process(u8 *buf, u32 len)
{
	u32 i;
	ARP_HDR *arp_p = (ARP_HDR*)buf;

	if ( packet_len < 28 )
	{
		return (0);
	}

	switch (HON(arp_p->opcode))
	{
		/*arp��Ӧ��*/
		case 2:
			memcpy(host_ip_addr, arp_p->sipaddr, 4);
			printf("host ip is : ");
			for (i = 0; i < 4; i++)
			{
				printf("%03d ", host_ip_addr[i]);
			}
			printf("\n\r");

			memcpy(host_mac_addr, arp_p->smac, 6);
			printf("host mac is : ");
			for (i = 0; i < 6; i++)
			{
				printf("%02x ", host_mac_addr[i]);
			}

			printf("\n\r");
			break;
		/*arpӦ���*/
		case 1:
			memcpy(arpbuf.ethhdr.d_mac, arp_p->smac, 6);
			memcpy(arpbuf.ethhdr.s_mac, mac_addr, 6);
			arpbuf.ethhdr.type = HON(0x0806);   /*0x0806����ʾARP����*/
			arpbuf.hwtype =HON(1);
			arpbuf.protocol = HON(0x0800);
			arpbuf.hwlen = 6;
			arpbuf.protolen = 4;
			arpbuf.opcode = HON(2);
			memcpy(arpbuf.smac, mac_addr, 6);
			memcpy(arpbuf.sipaddr, ip_addr, 4);
			memcpy(arpbuf.dmac, arp_p->smac, 6);
			memcpy(arpbuf.dipaddr, arp_p->sipaddr, 4);

			dm9000_tx(&arpbuf, packet_len);
			break;

		default:
			return (0);
			break;
	}

}


/*�����յ���tftp��*/
void udp_process(u8 *buf, u32 len)
{
	UDP_HDR *udphdr = (UDP_HDR*)buf;

	tftp_process(buf, len, HON(udphdr->sport));
}

void ip_process(u8 *buf, u32 len)
{
	IP_HDR *P = (IP_HDR*)buf;

	switch (P->proto)
	{
		/*UDPЭ��*/
		case PROTO_UDP:
			udp_process(buf, len);
			break;

		default:
			break;
	}
}

void net_handle(u8 *buf, u32 len)
{
	ETH_HDR *P = (ETH_HDR*)buf;

	switch ( HON(P->type) )
	{
		/*ARP��*/
		case PROTO_ARP:
			arp_process(buf, len);
			break;

		/*IP��*/
		case PROTO_IP:
			ip_process(buf, len);
			break;

		default:
			break;
	}
}











