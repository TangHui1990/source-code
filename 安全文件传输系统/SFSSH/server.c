#include "server.h"

#define THREADNUM 20

static int MaxClientNum=0;  //客服端最大连接数
static int CurrentClientNum=0; //当前客服端连接数
static int IsRun=0; //判断服务器是否开启
static int IsExit=0; //判断服务器是否退出

static int ThreadIdleId[THREADNUM]={0};			/*靠靠靠靠靠靠*/ 
static int ThreadBusyId[THREADNUM]={0};			/*靠靠靠靠靠靠*/ 
static int TaskId[THREADNUM]={0};
static int TIdleNum=0;  //ThreadBusyId数组的序列号
static int TBusyNum=0;
static int TaskNum=0;
pthread_t id;                //线程号
pthread_mutex_t pthreadMutex=PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t pthreadCond=PTHREAD_COND_INITIALIZER; //初始化条件变量

char Admin[1024];  //记录管理员账号和密码
char User[1024];   //记录用户账号和密码
char clientIp[15]; //记录用IP
char filelist[1024]; //文件列表
SSL_CTX *ctx;
int sockfd;
int new_fd;
int tempsockfd;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
int sin_size;
int portnumber=3333;
char userdir[50]="\0";

struct FilePacKage buff;  //接受客服端数据包

/*-----------------------处理函数--------------------*/
void InitAU();             //将管理员和用户的信息读入数组
int InitMaxClientNum();    //初始化最大客服端连接数
void mainMenu();           //主菜单处理函数  
void mainThread();          //主线程处理函数
void process();              //用户处理函数
struct FilePacKage unpack(SSL *,struct FilePacKage );  //解包函数
int CheckClient(char* ,char* );  //检测用户名和密码是否正确
void getlist(char *);            //获取文件列表函数
void receivePipBroken(int );      //捕获pipe断开异常
char *getCurrentTime();            //获取系统当前时间
void CreateThreadPool();           //创建线程池
void MOveToIdle(int );             //线程执行结束后把自己放入空闲线程中
void MoveToBusy(int );              //线程开始执行时把自己放入忙碌线程中
void AddTask(int );                 //把任务添加到线程池中

/*------------------------主函数--------------------------*/

int main(int argc,char *argv[])
{

 pthread_t controlId;
 pthread_t mainId;

 MaxClientNum=InitMaxClientNum();  //初始化客户端最大连接数

 InitAU();

 
	printf("\033[36m***********\033[0m\033[34mWelcome to Secure File Storage System\033[0m\033[36m**************\n\033[0m");
	printf("\033[36m*\033[0m \033[31m        ******     ******     ******     ******   \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m       **          *         **         **        \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m        *****      ******     *****      *****    \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m            **     *              **         **   \033[0m  \033[36m       *\n\033[0m\033[0m");
	printf("\033[36m*\033[0m \033[31m       ******      *         ******     ******  \033[0m \033[34mKJC\033[0m  \033[36m*\n\033[0m\033[0m");
	printf("\033[36m**************************************************************\n\033[0m");
 

int flag=1,nflag=1;  //flag=1表示登陆错误，flag=0表示登陆成功
char AdminName[20]="\0";
char AdminPwd[20]="\0";
char AN[20]="\0";
char AP[20]="\0";
char *pAdmin=Admin;
int i=0;

while(flag!=0)
{
printf("Admin Id:");
scanf("%s",AdminName);
printf("Admin PassWord:");
scanf("%s",AdminPwd);

while(nflag!=0)
{

while(*pAdmin!='*'&&(*pAdmin)!='\0')
{
if(i<20)
{
AN[i]=*(pAdmin);
++i;
++pAdmin;
}
}

++pAdmin;
i=0;
while(*pAdmin!='#'&&(*pAdmin)!='\0')
{
if(i<20)
{
AP[i]=*(pAdmin);
++i;
++pAdmin;
}
}
++pAdmin;
if(strcmp(AN,AdminName)==0)
{
nflag=0;
if(strcmp(AP,AdminPwd)==0)
{
flag=0;
printf("\n\033[33mlogin success!\033[0m\n\n");
break;
}

else
{
nflag=1;
memset(AP,'\0',20);
memset(AN,'\0',20);
pAdmin=Admin;
printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
i=0;
break;
}
}

else if(*pAdmin=='\0')
{
pAdmin=Admin;
i=0;
memset(AP,'\0',20);
memset(AN,'\0',20);
printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
break;
}

i=0;
memset(AP,'\0',20);
memset(AN,'\0',20);
}
}//while 结束

if((pthread_create(&controlId,NULL,(void *)mainMenu,NULL))!=0)
{
printf("\033[33mCreate menu error!\033[0m\n");
}

if((pthread_create(&mainId,NULL,(void *)mainThread,NULL))!=0)
{
printf("\033[33mCreate menu error!\033[0m\n");
}


pthread_join(controlId,NULL);
pthread_join(mainId,NULL);
return 0;
}

//菜单函数
void mainMenu()
{
int choice;
int cConfig;
int flag=1;

int fdMax;
int fdAdmin;
int fdUser;

char NewAd[20]="\0";
char NewAdPwd[20]="\0";
char Wadmin[45]="\0";

if((fdMax=open("./maxclientnum.txt",O_RDWR))==-1)
{
printf("\033[31mmaxclientnum.txt open error!\033[0m\n");
exit(-1);
}

if((fdAdmin=open("./admin.txt",O_WRONLY|O_APPEND))==-1)
{
printf("\033[31madmin.txt file open error!\033[0m\n");
exit(-1);
}

if((fdUser=open("./user.txt",O_WRONLY|O_APPEND))==-1)
{
printf("\033[31muser.txt file open error!\033[0m\n");
exit(-1);
}

while(1)
{
    printf("  \033[34m***********Server console***********\033[0m\n");
    printf("  \033[34m*\033[0m          1.Configure             \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          2.Run server            \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          3.Stop server           \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          4.Show status           \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          5.Exit                  \033[34m*\033[0m\n");
    printf("  \033[34m************************************\033[0m\n");

    printf("  please input the server command number:");
    scanf("%d",&choice);
    system("clear");

    switch(choice)
    {
    case 1:                    //服务器配置
            {
               flag=1;
               while(flag!=0)
               {

               
					printf("  \033[34m***************Configure**************\033[0m\n");
					printf("  \033[34m*\033[0m        1.Set maximum client        \033[34m*\033[0m\n");
					printf("  \033[34m*\033[0m        2.Add admin account         \033[34m*\033[0m\n");
					printf("  \033[34m*\033[0m        3.Add client account        \033[34m*\033[0m\n");
					printf("  \033[34m*\033[0m        4.Go back                   \033[34m*\033[0m\n");
				        printf("  \033[34m**************************************\033[0m\n");
				
	printf("  please input the configuration command number:");
					scanf("%d",&cConfig);
					system("clear");

					switch(cConfig)
					{
					case 1:                  //设置最大服务器连接数
					       {
					       int changeMax;
					       while(flag!=0)
					       {
					            printf("\033[33mthe current max client num is:\033[31m%d\n",MaxClientNum);
					            printf("\033[34minput the max num U want change:");
					            scanf("%d",&changeMax);
					            if(changeMax<0)
					            {
					            printf("\033[34mU input the num is below 0\n");
					            }
					            else
					            {
					            char wr[3];
					            flag=0;
					            MaxClientNum=changeMax;
					            sprintf(wr,"%d",MaxClientNum);

					            if(write(fdMax,wr,strlen(wr))<0)
					            {
					            printf("write to file error!\n");
					            exit(-1);
					            }
					            printf("\033[34mthe new max client num is:\033[31m%d\n",MaxClientNum);
					            }
					            
					       }
					       }
					 case 2:                        //新增管理员账号
					        {
					              printf("\033[34minput the new admin name:");
					              scanf("%s",NewAd);
					              printf("\033[34minput the new admin passwd:");
					              scanf("%s",NewAdPwd);
					              strcat(Wadmin,NewAd);
					              strcat(Wadmin,"*");
					              strcat(Wadmin,NewAdPwd);
					              strcat(Wadmin,"#");
					              if(write(fdAdmin,Wadmin,strlen(Wadmin))<0)
					              {
					              perror("  \033[31mwrite admin.txt error\033[0m\n");
					              exit(1);
					              }
					              else
					              {
					              printf("\033[31madd admin success!\n");
					              }
					        }
					        break;

					 case 3:                      //新增用户账号
					          {
					               char tuserdir[50]="\0";
					               printf("\033[34minput the new user name:");
					               scanf("%s",NewAd);
					               printf("\033[34minput the new user passwd:");
					               scanf("%s",NewAdPwd);
					               strcat(Wadmin,NewAd);
					               strcat(Wadmin,"*");
					               strcat(Wadmin,NewAdPwd);
					               strcat(Wadmin,"#");
					               strcat(tuserdir,"./");
					               strcat(tuserdir,NewAd);

					               if(write(fdUser,Wadmin,strlen(Wadmin))<0)
					               {
					               perror("  \033[31mwrite user.txt error\033[0m\n");
					               exit(1);
					               }
					               else
					               {
					               printf("\033[31madd user success!\n");
					               }
					               if(mkdir(tuserdir,0777)==-1)
					               {
					               perror("mkdir error:\n");
					               }

					               
					          }
					          break;

					  case 4:
					           { 
					             flag=0;
					             break;
					        
					           }
					           break;
					}
               }
            }
            break;
    case 2:              //运行服务器
        {
            IsRun=1;
            printf("\n\033[32mserver is running now\033[0m\n\n");
            break;
        }
        break;

    case 3:                    //关闭服务器
        {
            IsRun=0;
            printf("\n\033[32mserver is stop now\033[0m\n\n");
            break;
        }
        break;

    case 4:
            {
                system("clear");  //系统日记

                printf("\n\033[31m-----------------System Log-----------------\033[0m\n\n");

                FILE *fpLog;

                if((fpLog=fopen("./log.txt","r"))==NULL)
                {
                printf("the log.sys file lost!\n");
                }
                char logInfo[200];
                while(fgets(logInfo,200,fpLog)!=NULL)
                {
                printf("\n\033[32m%s\033[0m",logInfo);
                }
                printf("\n");
                printf("\n\033[31m--------------------Lof End----------------\033[0m\n\n");
            }
            break;
    case 5:
            {
            IsExit=1;
            exit(1);
            }
            break;

            
            
    }
    
}
close(fdMax);
close(fdAdmin);
close(fdUser);
}



/*主线程处理函数*/
void mainThread()
{
int reuse=1;
int i=0;
signal(SIGPIPE,receivePipBroken);


/*---------------------SSL--------------------*/
char pwd[100];
char *temp;
//SSL库初始化
SSL_library_init();
//载入所有SSL算法
OpenSSL_add_all_algorithms();
//载入所有SSL错误消息
SSL_load_error_strings();
//以SSL V2和V3标准兼容方式产生一个SSL_CTX
ctx=SSL_CTX_new(SSLv23_server_method());

if(ctx==NULL)
{
ERR_print_errors_fp(stdout);
exit(1);
}

//载入用户的数字证书，此证书用来发送给客服端，证书里包含公钥
getcwd(pwd,100);
if(strlen(pwd)==1)
pwd[0]='\0';
if(SSL_CTX_use_certificate_file(ctx,temp=strcat(pwd,"/cacert.pem"),SSL_FILETYPE_PEM)<=0)
{
ERR_print_errors_fp(stdout);
exit(1);
}

//载入用户私钥
getcwd(pwd,100);
if(strlen(pwd)==1)
pwd[0]='\0';

if(SSL_CTX_use_PrivateKey_file(ctx,temp=strcat(pwd,"/privkey.pem"),SSL_FILETYPE_PEM)<=0)
{
ERR_print_errors_fp(stdout);
exit(1);
}

//检查用户私钥是否正确
if(!SSL_CTX_check_private_key(ctx))
{
ERR_print_errors_fp(stdout);
exit(1);
}


/*-------------------------SSL------------------------------*/

if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
{
fprintf(stderr,"\033[33mSocket error:%s\033[0m\n\a",strerror(errno));
exit(-1);
}

bzero(&server_addr,sizeof(struct sockaddr_in));
server_addr.sin_family=AF_INET;
server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
server_addr.sin_port=htons(portnumber);
setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
if(bind(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr))<0)
{
fprintf(stderr,"\033[33mBind error:%s\033[0m\n\a",strerror(errno));
exit(1);
}

if(listen(sockfd,MaxClientNum)==-1)
{
fprintf(stderr,"\033[33mListrn error:%s\033[0m\n\a",strerror(errno));
exit(1);
}

CreateThreadPool();//创建线程池


while(1)
{
if(IsExit==1)
{
int i;
printf("\033[31m\nclosing server\n\033[0m");

for(i=0;i<60;i+=10)
{
printf("\033[31m.\033[0m");
fflush(stdout);
usleep(100000);
}
printf("\033[31m\nserver closedd\n\033[0m");
pthread_exit(0);
}

if(IsRun==1)
{
if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)
{
perror("accept error!");
exit(-1);
}

tempsockfd=new_fd;
strcpy(clientIp,inet_ntoa(client_addr.sin_addr));

pthread_cond_signal(&pthreadCond);
}
}

close(sockfd);
SSL_CTX_free(ctx);

pthread_join(id,NULL);

}


void process()
{

tap:      pthread_mutex_lock(&pthreadMutex);
          pthread_cond_wait(&pthreadCond,&pthreadMutex);
          pthread_mutex_unlock(&pthreadMutex);

int new_fd=tempsockfd;
struct FilePacKage sendPackage;
SSL *ssl;

//基于ctx产生一个新的SSL
ssl=SSL_new(ctx);
//将连接用户的socket加入到SSL
SSL_set_fd(ssl,new_fd);

//建立SSL连接，也就是我们常说的握手
if(SSL_accept(ssl)==-1)
{
perror("accept");
close(new_fd);
}

SSL *NewFd=ssl;
++CurrentClientNum;

if(CurrentClientNum>MaxClientNum)  //客服端连接数达到最大
{
sendPackage=pack('L'," "," ",0,2,1,"");
SSL_write(NewFd,&sendPackage,sizeof(struct FilePacKage));
--CurrentClientNum;
//关闭SSL连接
SSL_shutdown(NewFd);
//释放SSL
SSL_free(NewFd);
close(new_fd);
}


while(1)
{
SSL_read(NewFd,&buff,sizeof(struct FilePacKage));
if(buff.cmd=='Q'&&buff.ack=='0')
{
//关闭SSL连接
SSL_shutdown(NewFd);

//释放SSL
SSL_free(NewFd);
close(new_fd);
break;
}

else
{
sendPackage=unpack(NewFd,buff);
if(sendPackage.cmd!='\0')
{
SSL_write(NewFd,&sendPackage,sizeof(struct FilePacKage));
}
}

}

--CurrentClientNum;
goto tap;
}


//解包函数

struct FilePacKage unpack(SSL *NewFd,struct FilePacKage tpack)
{
struct FilePacKage sendPack;
char username[20]="\0";
char userpwd[20]="\0";
char *pUser=tpack.buf;
char pfilename[125]="\0";
int filesize;
int currentFsize=0;
int fd;
int fdlog;

int flag=1;

switch(tpack.cmd)
{
case 'L':         //登陆请求
        {
        int i=0;
        while(*pUser!='*')    //从数据包中读取用户名和密码
        {
        if(i<20)
        {
        username[i]=*pUser;
        ++i;
        ++pUser;
        }
        }

        ++pUser;  //跳过*
        i=0;

        while(*pUser!='#')
        {
        if(i<20)
        {
        userpwd[i]=*pUser;
        ++i;
        ++pUser;
        }
        }

        if(CheckClient(username, userpwd)==1)
        {

        sendPack=pack('L',"","",0,1,1,"");
        strcpy(filelist,"");
        }

        else
        {
        sendPack=pack('L',"","",0,0,1,"");
        }
        return sendPack;
        }
        break;
case 'U':
        {
        struct statfs statfsbuf;
        int count=0;
        currentFsize=0;

        if(tpack.ack==9)
        {
        strcat(pfilename,tpack.username);
        strcat(pfilename,"/");
        strcat(pfilename,tpack.filename);
        filesize=tpack.filesize;
        //文件名存在以后实现
        statfs("./",&statfsbuf);
        if((statfsbuf.f_bsize*statfsbuf.f_bfree)<=filesize)
        {
        printf("\033[31m磁盘空间不足\033[0m\n");
        sendPack=pack('U',"","",0,1,1,"");
        SSL_write(NewFd,&sendPack,sizeof(struct FilePacKage));
        exit(1);
        }

        if((fd=open(pfilename,O_RDWR|O_CREAT,0777))<0)
        {
        perror("open error:\n");
        }

        sendPack=pack('U',"","",0,0,1,"");
        SSL_write(NewFd,&sendPack,sizeof(struct FilePacKage));

        if((count=SSL_read(NewFd,&buff,sizeof(struct FilePacKage)))==-1)
        {
        perror("read error:\n");
        }

        while(buff.ack==2)
        {
        count=buff.filesize;
        if(write(fd,buff.buf,count)==-1)
        {
        perror("write error:\n");
        }

        if(SSL_read(NewFd,&buff,sizeof(struct FilePacKage))==-1)
        {
        perror("read error:\n");
        }
        }//文件上传结束
       //写入日记

       char Log[200]="\0";
       strcat(Log,"");
       strcat(Log,clientIp);

       strcat(Log,"\n");

       strcat(Log,"upload Date: ");
       strcat(Log,getCurrentTime());

       strcat(Log,"file Name:");
       strcat(Log,pfilename);
       strcat(Log,"\n");

       if((fdlog=open("./log.txt",O_WRONLY|O_APPEND))==-1)
       {
       printf("\033[33mlog.sys file open error!\033[0m\n");
       exit(-1);
       }

        if((write(fdlog,Log,strlen(Log)))<0)
        {
        perror("write long.txt error:\n");
        exit(-1);
        }
        close(fdlog);

        if(buff.ack==4)
        {
        sendPack=pack('U',"","",0,3,1,"");
        close(fd);
        }
        return sendPack;
        }
        
        }
        break;
case 'S':
        {
        getlist(tpack.username);
        sendPack=pack('S',filelist,"",0,1,strlen(filelist)+1,"");

        strcpy(filelist,"");
        return sendPack;
        }
        break;
        

case 'D':
        {
        int Fd;
        char buf[1025]={'\0'};
        int count=0;
        struct stat statbuf;
        char filename[125]="\0";
        struct FilePacKage data;

        if(tpack.ack==9)
        {
        strcat(filename,tpack.username);
        strcat(filename,"/");
        strcat(filename,tpack.filename);
        }

        if(stat(filename,&statbuf)==-1)
        {
        sendPack=pack("D","","",0,8,1,"");
        return sendPack;
        }
        sendPack=pack('D',"",filename,statbuf.st_size,0,1,"");
        SSL_write(NewFd,&sendPack,sizeof(struct FilePacKage));

        if((Fd=open(filename,O_RDONLY))==-1)
        {
        perror("open error:\n");
        }

        while((count=read(Fd,(void *)buf,1024))>0)
        {
        sendPack=pack('D',buf,filename,count,2,count,"");
        if((SSL_write(NewFd,&sendPack,sizeof(struct FilePacKage)))==-1)
        {
        perror("send login message:");
        }
        }

        sendPack=pack('D',"",filename,statbuf.st_size,4,1,"");
        SSL_write(NewFd,&sendPack,sizeof(struct FilePacKage));
        SSL_read(NewFd,&data,sizeof(struct FilePacKage));
        if(data.cmd=='D'&&data.ack==3)
        {
        sendPack=pack('\0'," "," ",0,8,1,"");
        close(Fd);
        }

        //写入日记

        char Log[200]="\0";

        strcat(Log,"");
        strcat(Log,"client IP:");
        strcat(Log,clientIp);

        strcat(Log,"\n");

        strcat(Log,"download Date: ");
        strcat(Log,getCurrentTime());

        strcat(Log,"File Name:");
        strcat(Log,filename);

        strcat(Log,"\n");

        if((fdlog=open("./log.txt",O_WRONLY|O_APPEND))==-1)
        {
        printf("\033[33mlog.txt file open error!\033[0m\n");
        exit(-1);
        }

        if((write(fdlog,Log,strlen(Log)))<0)
        {
        perror("write long.txt error:\n");
        exit(-1);
        }
        close(fdlog);
        return sendPack;
        }
        break;
            
}


}

//获取文件列表

void getlist(char *username)
{
DIR *pdir;
struct dirent *pent;
char DEFDIR[60]="\0";

strcpy(filelist," ");
strcat(DEFDIR,"./");
strcat(DEFDIR,username);

if((pdir=opendir(DEFDIR))==NULL)
{
fprintf(stderr,"open dir error\n");
return ;
}

while(1)
{
pent=readdir(pdir);
if(pent==NULL)
{
break;
}
else
{
strcat(filelist,pent->d_name);
strcat(filelist,"\t");
}
}

closedir(pdir);
}


char *getCurrentTime()
{
time_t now;
struct tm *timenow;
time(&now);
timenow=localtime(&now);
return asctime(timenow);
}

int CheckClient(char *tUser, char *tPwd)
{
int flag=1,nflag=1;

char UN[20]="\0";
char UP[20]="\0";
char *pUser=User;
int i=0;

while(flag!=0)
{
while(nflag!=0)
{
while(*pUser!='*'&&(*pUser)!='\0')
{
if(i<20)
{
UN[i]=*(pUser);
++i;
++pUser;
}
}

++pUser;
i=0;
while(*pUser!='#'&&(*pUser)!='\0')
{
if(i<20)
{
UP[i]=*(pUser);
++i;
++pUser;
}
}

++pUser;
if(strcmp(UN,tUser)==0)
{
nflag=0;
if(strcmp(UP,tPwd)==0)
{
flag=0;
return 1;
}
else
{
nflag=1;
memset(UP,'\0',20);
memset(UN,'\0',20);
pUser=User;
printf("\n\033[33mUser name or passwd is error!\033[0m\n\n");
i=0;
return 0;
}
}

else if(*pUser=='\0')
{
pUser=User;
i=0;
memset(UP,'\0',20);
memset(UN,'\0',20);
printf("\n\033[33mUser name or passwd is error!\033[0m\n\n");
return 0;
}
i=0;
memset(UP,'\0',20);
memset(UN,'\0',20);
}
}//while结束
}



void CreateThreadPool()
{
int i;
for(i=0;i<THREADNUM;i++)
{
pthread_t tid=0;
pthread_create(&tid,NULL,(void *)process,NULL);
ThreadIdleId[TIdleNum]=tid;
++TIdleNum;
}
}


void receivePipBroken(int sign_no)
{
if(sign_no==SIGPIPE)
{
printf("\n\033[31ma client exit!\033[0m\n\n");
printf("\n\033[33mplease choose a command:\033[0m\n\n");
CurrentClientNum--;
pthread_exit(0);
}
}

/* 初始化客服端最大连接数*/
int InitMaxClientNum()
{
int ReadNum;
int fd;
char buf[10];

if((fd=open("./maxclientnum.txt",O_RDONLY))<0)
{
printf("can not open maxclientnum.txt!");
}

if((ReadNum=read(fd,buf,10))<0)
{
printf("can not read from maxclientnum.txt!");
}

close(fd);
return atoi(buf);
}

//初始化管理员和用户

void InitAU()
{
int nRead;
int fd;
char *pAdmin=Admin;
char *pUser=User;
int nLeft=1024;


if((fd=open("./admin.txt",O_RDONLY))<0)
{
printf("can not open admin.txt!");
}

while(nLeft>0)
{
if((nRead=read(fd,pAdmin,20))<0)
{
printf("can not read from admin.txt!");
}
else if(nRead==0)
{
break;
}
pAdmin+=nRead;
nLeft-=nRead;
}
close(fd);


//从user.txt中读取用户信息到数组
if((fd=open("./user.txt",O_RDONLY))<0)
{
printf("can not open user.txt!");
}

while(nLeft>0)
{
if((nRead=read(fd,pUser,20))<0)
{
printf("can not read from user.txt!");
}

else if(nRead==0)
{
break;
}
pUser+=nRead;
nLeft-=nRead;
}

close(fd);
}


//线程执行结束以后，把自己放入空闲线程中

void MOveToIdle(int tid)
{
int i=0;
int j=0;

while(i<=TBusyNum)
{
if(ThreadBusyId[i]==tid)
{
//移除此tid在busy数组中

if(i!=TBusyNum)
{
for(j=i+1;j<=TBusyNum;++j)
{
ThreadBusyId[j-1]=ThreadBusyId[j];
}
}
--TBusyNum;
break;
}
++i;
}
//把tid增加到idle数组中
ThreadIdleId[TIdleNum]=tid;
++TIdleNum;
}


//移入到忙碌线程中去

void MoveToBusy(int tid)
{
int i=0;
int j=0;

while(i<=TIdleNum)
{
if(ThreadIdleId[i]==tid)
{
//移除此tid在idle数组中
if(i!=TIdleNum)
{
for(j=i+1;j<=TIdleNum;++j)
{
ThreadIdleId[j-1]=ThreadIdleId[j];
}
}
--TIdleNum;
break;
}
++i;
}

ThreadBusyId[TBusyNum]=tid;
++TBusyNum;
}


//往线程池中添加任务
void AddTask(int tid)
{
TaskId[TaskNum]=tid;
++TaskNum;
pthread_cond_signal(&pthreadCond);
}













