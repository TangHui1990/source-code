#ifndef __ARP_H__
#define __ARP_H__

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/*��̫���ײ�*/
typedef struct eth_hdr
{
	u8 d_mac[6];     /*��̫��Ŀ�ĵ�ַ*/
	u8 s_mac[6];    /*��̫��Դ��ַ*/
	u16 type;         /*֡����*/
}ETH_HDR;

/*28�ֽ�ARP����/Ӧ���*/
typedef struct arp_hdr
{
	ETH_HDR ethhdr;  /*ARP����IP��Э�飬�ʷ�װ���²���̫�����ײ�*/
	u16 hwtype;        /*Ӳ������*/
	u16 protocol;      /*Э������*/
	u8 hwlen;          /*Ӳ����ַ����*/
	u8 protolen;       /*Э���ַ����*/
	u16 opcode;       /*op=1 ARP�������op=2 ARPӦ���*/
	u8 smac[6];       /*���Ͷ���̫����ַ*/
	u8 sipaddr[4];     /*���Ͷ�IP��ַ*/
	u8 dmac[6];       /*Ŀ�Ķ���̫����ַ*/
	u8 dipaddr[4];    /*Ŀ�Ķ�IP��ַ*/
}ARP_HDR;

/*IPͷ*/
typedef struct ip_hdr
{
	/*ipЭ������IP��ʷ�װ�³���̫��ͷ*/
	ETH_HDR ethhdr;
	u8 vhl; /*4λ�汾,4λ�ײ�����*/
	u8 tos; /*��������*/
	u16 len; /*�����ܳ���*/
	u16 ipid; /*16λ��ʶ*/
	u16 ipoffset; /*ƫ��*/
	u8 ttl; /*����ʱ��*/
	u8 proto; /*Э��*/
	u16 ipchksum; /*�ײ�У���*/
	u8 srcipaddr[4]; /*ԴIP��ַ*/
	u8 destipaddr[4]; /*Ŀ��ip��ַ*/
}IP_HDR;

/*UDPͷ*/
typedef struct udp_hdr
{
	IP_HDR iphdr;/*UDP���ڴ����Э�飬Ӧ�����²�IPͷ*/
	u16 sport;  /*Դ�˿�*/
	u16 dport;  /*Ŀ�ĵض˿�*/
	u16 len;    /*���ĳ���*/
	u16 udpchksum ;/*У��λ*/
}UDP_HDR;

/*tftp���ݱ���*/
typedef struct tftp_package
{
	u16 opcode;   /*������*/
	u16 blocknum; /*����*/
	u8 data[0];
}TFTP_PAK;

ARP_HDR arpbuf;

extern u8 host_mac_addr[6];
extern u8 mac_addr[6];
extern u8 ip_addr[4];
extern u8 host_ip_addr[4];
u16 packet_len;

#define PROTO_ARP 0x0806   //ARP������
#define PROTO_IP 0x0800    //IP������
#define PROTO_UDP 0x11    //UDP������

#endif












