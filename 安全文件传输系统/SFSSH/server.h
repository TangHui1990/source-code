#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <sys/file.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct FilePacKage //协议结构体
{
char cmd; //操作命令
int filesize; //每次传输数据包的大小
int ack;//标志位
char username[50]; //客服端用户名
char filename[125]; //传输文件名
char buf[1024]; //传输文件的元数据
};

//打包函数 
struct FilePacKage pack(char tCmd,char* tBuf,char* tFilename,int tFilesize,int tAck,int count,char *uname)
{
struct FilePacKage tPackage;
tPackage.cmd=tCmd;
memcpy(tPackage.buf,tBuf,count);
strcpy(tPackage.filename,tFilename);
strcpy(tPackage.username,uname);
tPackage.filesize=tFilesize;
tPackage.ack=tAck;
return tPackage;
}




