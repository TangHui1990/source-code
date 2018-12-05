#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <error.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <xl_debug.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**************************net transfer****************************/
typedef struct
{
	int yuvflg;//是否输出yuv图像
	int verbose;//是否输出信息
	int yuvcount;//第几张yuv图片
	int NetOutE;//
	char ip[32];
	int port;
	FILE *fp;
	//sendto中使用的对方地址
	struct sockaddr_in toAddr;
	int sock;
}DebugStr;DebugStr debugs;

static int SearchArgv(char *src, char *cmp, char *buff, int len)
{
	char *i = NULL;
	char *j = NULL;
	if ((i = strstr(src,cmp)) == NULL)
	{
		return 1;
	}
	
	i = i + strlen(cmp);
	if((j = strstr(i,";")) == NULL)
	{
		return 2;
	}
	
	if((int )(j - i) > len)
	{
		return 3;
	}
	
	memcpy(buff,i,(int)(j-i));
	buff[(int)(j-i)] = '\0';
	
	return 0;
}
	
void InitDebufInfo(void)
{	
	debugs.fp = fopen("./isp.ini","r");
	if (debugs.fp == NULL)
	{
		fopen("./isp.ini","wr+");
	}
	
	if (debugs.fp != NULL)
	{
		char buff[4096];
		char result[128];
		int nread = 0;
		memset(buff,0,sizeof(buff));
		nread = fread(buff,1,sizeof(buff),debugs.fp);
		if (nread > 0)
		{
			if (strstr(buff,"VERBOSE=1;") != NULL)
			{
				debugs.verbose = 1;//输出信息
			}
			if (strstr(buff,"YUV=1;") != NULL)
			{
				debugs.yuvflg = 1;
			}
			if (strstr(buff,"YUV=2;") != NULL)
			{
				debugs.yuvflg = 2;
			}
			if (strstr(buff,"NetOutE=0;") != NULL)
			{
				debugs.NetOutE = 0;
			}else
			{
				debugs.NetOutE = 1;

			}
			if(debugs.NetOutE)
			{
				memset(result,0,sizeof(result));
				if(SearchArgv(buff,"ip=",result,sizeof(result)) == 0 )
				{
						memcpy(debugs.ip,result,strlen(result));
				}
				memset(result,0,sizeof(result));
				if(SearchArgv(buff,"port=",result,sizeof(result)) == 0 )
				{
                    debugs.port = atoi(result);

				}
				debugs.sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
				XL_TRACE_DEBUG("yuv output ip:%s ,port:%d\n",debugs.ip,debugs.port);

				if(debugs.sock < 0)
				{
						XL_TRACE_DEBUG(" 创建套接字失败了.\r\n");
						exit(1);
				}
				
				memset(&(debugs.toAddr),0,sizeof(debugs.toAddr));
				debugs.toAddr.sin_family=AF_INET;
				debugs.toAddr.sin_addr.s_addr=inet_addr(debugs.ip);
				debugs.toAddr.sin_port = htons(debugs.port);
			}
		}
		fclose(debugs.fp);
	}

	if (debugs.yuvflg == 2)
	{

	}
}

static int num = 1;
static FILE *fp;
int OutTest(char *data, int width_r, int height_r)
{
	num++;
	char yuvname[128];
	memset(yuvname,0,sizeof(yuvname));
	sprintf(yuvname,"/test/%dsucceed.yuv",num);
	
	fp = fopen(yuvname,"w");
	if (fp != NULL)
	{
		fwrite(data,1,width_r * height_r,fp);
		fclose(fp);
	}

	return (0);
}

#define PACKLEN 1024
int OutPutYuv(int f,char *data,int width_r,int height_r)
{
	debugs.yuvcount++;//
	if(debugs.yuvflg == 1)
	{
		char yuvname[128];
		memset(yuvname,0,sizeof(yuvname));
		if(f == 0)
		{
			sprintf(yuvname,"./%dsucceed.yuv",debugs.yuvcount);
		} 
		else 
		{
			sprintf(yuvname,"./%dFailed.yuv",debugs.yuvcount);
		}
		debugs.fp = fopen(yuvname,"w");
		if (debugs.fp != NULL)
		{
			fwrite(data,1,width_r * height_r,debugs.fp);
			fclose(debugs.fp);
		}

	}
	
	if (debugs.yuvflg == 2)
	{
		if (debugs.fp != NULL)
			fwrite(data,1,width_r * height_r,debugs.fp);

	}
	if(debugs.NetOutE)
	{
		int len = width_r*height_r;
		int data_p = 0;
		int sendlen = 0;
		int freamSum = 0;
		char sendbuf[2048];
		char recvbuf[2048];
		int i=0;
		freamSum = len/PACKLEN+(len%PACKLEN?1:0);

		if (f == 0)
		{
			sprintf(sendbuf,"Startfream;length=%d;freamSum=%d;width=%d;height=%d;packlen=%d;filename=%dsucceed.yuv",len,freamSum,width_r,height_r,PACKLEN,debugs.yuvcount);
		}
		else
		{

			sprintf(sendbuf,"Startfream;length=%d;freamSum=%d;width=%d;height=%d;packlen=%d;filename=%dFailed.yuv",len,freamSum,width_r,height_r,PACKLEN,debugs.yuvcount);
		}

		socklen_t  sin_len	=sizeof(debugs.toAddr);
		while(1)
		{
			if(sendto(debugs.sock,sendbuf,strlen(sendbuf),0,
						(struct sockaddr*)&(debugs.toAddr),sizeof(debugs.toAddr)) != strlen(sendbuf))
			{
				XL_TRACE_DEBUG("sendto() 函数使用失败了.\r\n");
				close(debugs.sock);
				exit(1);

			}
			memset(recvbuf,0,sizeof(recvbuf));

			usleep(1000);

			recvfrom(debugs.sock,recvbuf,sizeof(recvbuf),MSG_DONTWAIT,(struct sockaddr *)&(debugs.toAddr),&sin_len);
			if(strncmp(recvbuf,"Startfreamok",2) == 0)
			{
				//	XL_TRACE_DEBUG("recvbuf :%s\n",recvbuf);

				break;

			}
			i++;
			if(i  >= 20)
			{
				return -1;
			}
		}

		i = 0;
		while(len>0)
		{

			if(len > PACKLEN)
			{
				sendlen = PACKLEN;
				len -= sendlen;
			}
			else
			{
				sendlen = len;
				len -= sendlen;
			}

			memcpy(sendbuf,&i,4);
			//	char ssss[2048];
			//	memset(ssss,255,sizeof(ssss));
			memcpy(&sendbuf[4],data+data_p,sendlen);
			//	memcpy(&sendbuf[4],ssss,sendlen);

			if(sendto(debugs.sock,sendbuf,sendlen+4,0,
						(struct sockaddr*)&(debugs.toAddr),sizeof(debugs.toAddr)) != sendlen+4)
			{
				XL_TRACE_DEBUG("sendto1() 函数使用失败了\r\n");
				close(debugs.sock);
				exit(1);
			}

			i++;
			data_p+=sendlen;
		}
		//	XL_TRACE_DEBUG("send data count:%d\n",i);

		//ack end
		i = 20;
		len = width_r*height_r;
		while(i--)
		{
			if(sendto(debugs.sock,"endfream",strlen("endfream"),0,
						(struct sockaddr*)&(debugs.toAddr),sizeof(debugs.toAddr)) != strlen("endfream"))
			{
				XL_TRACE_DEBUG("sendto2() 函数使用失败了.\r\n");
				close(debugs.sock);
				exit(1);
			}
			memset(recvbuf,0,sizeof(recvbuf));

			usleep(1000);
			recvfrom(debugs.sock,recvbuf,sizeof(recvbuf),MSG_DONTWAIT,(struct sockaddr *)&(debugs.toAddr),&sin_len);

			if(strncmp(recvbuf,"look",4) == 0)
			{

				char *p = &recvbuf[4];

				int s = 0;

				while(1)
				{
					s = atoi(p);

					char *q;
					int Sendlen = 0;
					Sendlen = (len - PACKLEN*s)>=PACKLEN?PACKLEN:(len - PACKLEN*s);

					//	XL_TRACE_DEBUG("s = %d ,Sendlen:%d\n",s,Sendlen);
					memcpy(sendbuf,&s,4);
					//		char ssss[2048];
					//		memset(ssss,0,sizeof(ssss));

					//		memcpy(&sendbuf[4],ssss,Sendlen);
					memcpy(&sendbuf[4],data+PACKLEN*s,Sendlen);
					q = strstr(p,";");

					if(q == NULL)
					{
						// XL_TRACE_DEBUG("%s\n",p);

						break;
					}
					p = q+1;

					if(sendto(debugs.sock,sendbuf,Sendlen+4,0,
								(struct sockaddr*)&(debugs.toAddr),sizeof(debugs.toAddr)) != Sendlen+4)
					{
						XL_TRACE_DEBUG("sendto3() 函数使用失败了\r\n");
						close(debugs.sock);
						exit(1);
					}
				}


			}


			if(strncmp(recvbuf,"fullfreamok",8) == 0)
			{
				//	XL_TRACE_DEBUG("recvbuf :%s\n",recvbuf);
				break;

			}else{

			}

		}
	}

	return (0);
}
/*******************************************************************/

