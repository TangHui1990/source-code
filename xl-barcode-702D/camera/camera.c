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

#include <videodev2.h>
#include <HDSD.h>
#include <xl_debug.h>

#include <device.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define CAMERA_DEVICE	"/dev/video0"
int input = 0;

int camera_fd = -1;
struct Buffer *buffers = NULL;
int n_buffers = 0;

static void close_Camera(void)
{
	close(camera_fd);
}

static int xioctl(int fd, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fd, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

static int open_Camera(void)
{
	struct stat st;
	struct v4l2_capability cap;
	
	if (-1 == stat(CAMERA_DEVICE, &st)) 
	{
		XL_TRACE_DEBUG("Cannot identify %s error[%d]: %s\n", CAMERA_DEVICE, errno, strerror(errno));
		goto fail;
	}
	
	if (!S_ISCHR(st.st_mode))
	{
		XL_TRACE_DEBUG("%s is not device.\n", CAMERA_DEVICE);
		goto fail;
	}

	camera_fd = open(CAMERA_DEVICE, O_RDWR);
	if(camera_fd < 0)
	{
		XL_TRACE_DEBUG("open %s error[%d]: %s\n", CAMERA_DEVICE, errno, strerror(errno));
		goto fail;
	}

	/* 列举输入源，非必须 */
	if (ioctl(camera_fd, VIDIOC_S_INPUT, &input) < 0)
	{
		XL_TRACE_DEBUG("Get VIDIOC_S_INPUT error[%d]: %s\n", errno, strerror(errno));
		goto fail;
	}

	memset(&cap, 0, sizeof(cap));
	/* 表示它是一个摄像头设备 */
	if (ioctl(camera_fd, VIDIOC_QUERYCAP, &cap) < 0) 
	{   
		XL_TRACE_DEBUG("Get VIDIOC_QUERYCAP error[%d]: %s\n", errno, strerror(errno));
		goto fail;
	}
	
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) 
	{
		XL_TRACE_DEBUG("%s is not V4L2 device.\n", CAMERA_DEVICE);
		goto fail;
	}
	
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) 
	{
		XL_TRACE_DEBUG("%s does not support streaming I/O.\n", CAMERA_DEVICE);
		goto fail;
	}

	return 0;
	
fail:
	if(camera_fd >= 0)
		close(camera_fd);
	return -1;
}

static void init_userp(void)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size = 0;
	unsigned int buffer_size;

	page_size = getpagesize();
	buffer_size = (PREVIEW_WIDTH*PREVIEW_HEIGHT + page_size - 1) & ~(page_size - 1);
	CLEAR(req);

	req.count	= 1;
	req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory	= V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(camera_fd, VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno) 
		{
			exit(EXIT_FAILURE);
		} 
		else
		{
			exit(1);
		}
	}

	buffers = calloc(req.count, sizeof(*buffers));
	if (!buffers) 
	{
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

	#if 0
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = (unsigned char *)JZMalloc(128, buffer_size);
		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
	#endif

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) 
	{
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = (unsigned char *)valloc(buffer_size);

		if (!buffers[n_buffers].start) 
		{
			fprintf(stderr, "Out of memory\n");
			exit(1);
		}
	}

}

void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

		for (i=0; i<n_buffers; ++i)
		{
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.index = i;
			buf.m.userptr = (unsigned long)buffers[i].start;
			buf.length = buffers[i].length;
			if (-1 == xioctl(camera_fd, VIDIOC_QBUF, &buf))
			{
				printf("if (-1 == xioctl(camera_fd, VIDIOC_QBUF, &buf))\n");
				exit(1);
			}
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl(camera_fd, VIDIOC_STREAMON, &type))
		{
			exit(1);
		}

}

static int SetCamera_Format(void)
{
	struct v4l2_format fmt;
	
	memset(&fmt, 0, sizeof(fmt));

	fmt.type			= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix_mp.width		= PREVIEW_WIDTH;
	fmt.fmt.pix_mp.height		= PREVIEW_HEIGHT;
#ifdef CONFIG_SOC_CAMERA_AR0144
	fmt.fmt.pix_mp.pixelformat		= V4L2_PIX_FMT_GREY;
#endif

#ifdef CONFIG_SOC_CAMERA_MT9V022
	fmt.fmt.pix_mp.pixelformat		= V4L2_PIX_FMT_GREY;
#endif

#ifdef CONFIG_SOC_CAMERA_GC0308
	fmt.fmt.pix_mp.pixelformat		= V4L2_PIX_FMT_YUYV;
#endif

	//fmt.fmt.pix.priv = 1;
	//fmt.fmt.pix_mp.field		= V4L2_FIELD_ANY;  //V4L2_FIELD_INTERLACED

	/* 设置摄像头的格式 */
	if (ioctl(camera_fd, VIDIOC_S_FMT, &fmt) < 0) {
		XL_TRACE_DEBUG("VIDIOC_S_FMT error[%d]: %s\n", errno, strerror(errno));
		return -1;
	}
	
	return 0;
}

int stop_Capture(void)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (ioctl(camera_fd, VIDIOC_STREAMOFF, &type) < 0)
	XL_TRACE_DEBUG("VIDIOC_STREAMOFF error[%d]: %s\n", errno, strerror(errno));

	if(buffers > 0)
		free (buffers);

	return 0;
}

int readFrame(void)
{
	struct v4l2_buffer buf;
	unsigned int index;

	for (;;)
	{
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO (&fds);
		FD_SET (camera_fd, &fds);

		tv.tv_sec = 2;
		tv.tv_usec = 0;

		ret = select (camera_fd + 1, &fds, NULL, NULL, &tv);
		if (-1 == ret) 
		{
			if (EINTR == errno)
				continue;
			
			XL_TRACE_DEBUG("select error[%d]: %s\n", errno, strerror(errno));
			
			return -1;
		}
		else if (0 == ret)
		{
			XL_TRACE_DEBUG("ERROE: select timeout\n");
			return -1;
		}

		break;
	}

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//buf.memory = V4L2_MEMORY_MMAP;
	buf.memory = V4L2_MEMORY_USERPTR;
    /* 把缓冲区从队列中取出 */
	if (ioctl(camera_fd, VIDIOC_DQBUF, &buf) < 0) {
		XL_TRACE_DEBUG("VIDIOC_DQBUF error[%d]: %s\n", errno, strerror(errno));
		return -1;
	}

	index = buf.index;
	if (!(0 <= index && index < n_buffers)) {
		XL_TRACE_DEBUG("ERROR: Wrong get preview index.");
		return -1;
	}

	/* 把缓冲区放入队列 */
	if (ioctl(camera_fd, VIDIOC_QBUF, &buf) < 0)
		XL_TRACE_DEBUG("VIDIOC_QBUF error[%d]: %s\n", errno, strerror(errno));

	return index;
}

int camera_init(void)
{
	int ret;

	/* 打开摄像头 */
	ret = open_Camera();
	XL_RETURN(ret < 0, -1, "Open Camera Error!\n");

	/* 设置摄像头格式 */
	ret = SetCamera_Format();
	XL_RETURN(ret < 0, -1, "Set Camera Format Error!\n");

	/* 分配buffer */
	init_userp();

	/* 开启摄像头 */
	start_capturing();

	return (0);
}

void camera_exit(void)
{
	stop_Capture();

	close_Camera();
}



