#ifndef __ARP_H__
#define __ARP_H__

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/*以太网首部*/
typedef struct eth_hdr
{
	u8 d_mac[6];     /*以太网目的地址*/
	u8 s_mac[6];    /*以太网源地址*/
	u16 type;         /*帧类型*/
}ETH_HDR;

/*28字节ARP请求/应答包*/
typedef struct arp_hdr
{
	ETH_HDR ethhdr;  /*ARP属于IP层协议，故封装了下层以太网的首部*/
	u16 hwtype;        /*硬件类型*/
	u16 protocol;      /*协议类型*/
	u8 hwlen;          /*硬件地址长度*/
	u8 protolen;       /*协议地址长度*/
	u16 opcode;       /*op=1 ARP请求包，op=2 ARP应答包*/
	u8 smac[6];       /*发送端以太网地址*/
	u8 sipaddr[4];     /*发送端IP地址*/
	u8 dmac[6];       /*目的端以太网地址*/
	u8 dipaddr[4];    /*目的端IP地址*/
}ARP_HDR;

/*IP头*/
typedef struct ip_hdr
{
	/*ip协议属于IP层故封装下成以太网头*/
	ETH_HDR ethhdr;
	u8 vhl; /*4位版本,4位首部长度*/
	u8 tos; /*服务类型*/
	u16 len; /*报文总长度*/
	u16 ipid; /*16位标识*/
	u16 ipoffset; /*偏移*/
	u8 ttl; /*生存时间*/
	u8 proto; /*协议*/
	u16 ipchksum; /*首部校验和*/
	u8 srcipaddr[4]; /*源IP地址*/
	u8 destipaddr[4]; /*目的ip地址*/
}IP_HDR;

/*UDP头*/
typedef struct udp_hdr
{
	IP_HDR iphdr;/*UDP属于传输层协议，应包含下层IP头*/
	u16 sport;  /*源端口*/
	u16 dport;  /*目的地端口*/
	u16 len;    /*报文长度*/
	u16 udpchksum ;/*校验位*/
}UDP_HDR;

/*tftp数据报文*/
typedef struct tftp_package
{
	u16 opcode;   /*操作码*/
	u16 blocknum; /*块编号*/
	u8 data[0];
}TFTP_PAK;

ARP_HDR arpbuf;

extern u8 host_mac_addr[6];
extern u8 mac_addr[6];
extern u8 ip_addr[4];
extern u8 host_ip_addr[4];
u16 packet_len;

#define PROTO_ARP 0x0806   //ARP包类型
#define PROTO_IP 0x0800    //IP包类型
#define PROTO_UDP 0x11    //UDP包类型

#endif












