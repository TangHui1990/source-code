#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <error.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <linux/input.h>
#include <math.h>

#include <xl_debug.h>

#include <device.h>

int MoveDetect(unsigned char* p,unsigned char* q,int width,int height)
{
    int i,j,sum;

    for(j = 0,sum = 0; j < height;j+=10)
    {
        for(i = 0; i < width;i+=10)
        {
            unsigned char _v;
            int ret;
            int m;
            _v = (p[j*width+i]^q[j*width+i]);
            m=(!!(_v&0xF0))<<2;
            _v>>=m;
            ret =m;
			
            m=!!(_v&0xC)<<1;
            _v>>=m;
            ret|=m;
			
            ret|=!!(_v&0x2)<<(0);

            sum+= (1<<ret);
        }
    }
	
    return sum;
}

int XL_IsSetCodeInput(const char *pInputData, int iLength)
{	
	char *pTmpInputData = NULL;
	char *pret = NULL;
	char buf[7] = {'\0'};
	int i = 0;
	int iret = -1;
	int iCmdNum = 0;

	XL_RETURN(iLength != 13, 0, "The Input Not Set Code<iLength is not 13>");

	pTmpInputData = (char*)malloc(sizeof(iLength));
	XL_RETURN(pTmpInputData == NULL, -1, "malloc error!");

	memcpy(pTmpInputData, pInputData, iLength);

	pret = strchr(pTmpInputData, '$');
	if (pret == NULL)
	{
		free(pTmpInputData);
		XL_TRACE_DEBUG("The Input Not Set Code<not find $>");

		return (0);
	}

	for (i = 0; i < 6; i++)
	{
		buf[i] = pTmpInputData[i];
	}

	buf[6] = '\0';

	iret = strcmp(buf, "<0x02>");
	if (iret != 0)
	{
		free(pTmpInputData);
		XL_TRACE_DEBUG("The Input Not Set Code<not find <0x02>>");

		return (0);
	}
	
	if (pTmpInputData[6] != '$')
	{
		free(pTmpInputData);

		XL_TRACE_DEBUG("The Input Not Set Code<six bit is not $>");
		
		return (0);
	}

	memset(buf, '\0', sizeof(buf));

	for(i = 0; i < 4; i++)
	{
		buf[i] = pTmpInputData[i+7];
	}

	buf[4] = '\0';

	iCmdNum = atoi(buf);
	if ((iCmdNum>200)||(iCmdNum<0))
	{
		free(pTmpInputData);

		XL_TRACE_DEBUG("The Input Not Set Code<iCmdNum is not valid>");

		return (0);
	}

	free(pTmpInputData);

	return (1);
}

