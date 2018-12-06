#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <string.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <netinet/in.h>

#define port 3333

struct FilePacKage
{
char cmd;
int filesize;
int ack;
char username[50];
char filename[125];
char buf[1024];
};

struct FilePacKage pack(char tCmd,char *tBuf,char *tFilename,int tFilesize,int tAck,int count,char *uname)
{
struct FilePacKage tPacKage;
tPacKage.cmd=tCmd;
memcpy(tPacKage.buf,tBuf,count);
strcpy(tPacKage.filename,tFilename);
strcpy(tPacKage.username,uname);
tPacKage.ack=tAck;
tPacKage.filesize=tFilesize;
return tPacKage;
}



















