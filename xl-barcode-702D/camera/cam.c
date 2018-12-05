/*
 * V4L2 Spec Appendix B.
 *
 * Video Capture Example
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>	//getopt_long()
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>	//for videodev2.h
#include <linux/videodev2.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/input.h>
#include <semaphore.h>

#include <videodev2.h>
#include <HDSD.h>
#include <xl_debug.h>

#include <device.h>

typedef unsigned int	__u32;
typedef signed int	__s32;
typedef unsigned short	__u16;
typedef signed short	__s16;
typedef unsigned char	__u8;
typedef signed char	__s8;

struct camera_param{
    __u32 tlb	: 1,
          debug	: 1,
          format_spc: 1,
          save_jpeg_userptr :1,
          planar	: 1,
          reserved	: 26;


    __s32 cim_id;
    __u32 width;
    __u32 height;
    __u32 bpp;

};

struct camera_format {
    __u32 fourcc;	// compatible with V4L2
    __u32 format;	// local definition, more detail
};

struct camera_info{
    __s32 fd_cim;
    __s32 capture_limit;
    __s32 squeue;

    __u8  file_name[64];
    __u8  video_dev_name[16];

    struct camera_param param;
    struct camera_format fmt;

    __u32 yoff;
    __u32 xoff;

    __u8 *ybuf;
    __u8 *ubuf;
    __u8 *vbuf;
    __u32 n_bytes;

    void *jz_jpeg;
    void *buf_vaddr;
};

enum io_method {
    IO_METHOD_READ = 0,
    IO_METHOD_MMAP = 1,
    IO_METHOD_USERPTR = 2,
};

enum camera_options {
    OPS_PREVIEW = 0,
    OPS_CAPTURE = 1,
};

enum capture_options {
    OPS_SAVE_BMP = 0,
    OPS_SAVE_JPG = 1,
    OPS_SAVE_RAW = 2,
};

struct cim_ops {
    void (*capture_picture)(struct camera_info *camera_info, struct camera_ctl *camera_ctl);
    int (*priview_picture)(struct camera_info *camera_info);
    void (*camera_fps)(struct camera_info *camera_inf);
};

struct camera_ctl{
    /*v4l2 support  mode*/
    enum io_method io_method;
    /*camera optiongs*/
    enum camera_options cam_opt;
    /*capture optiong*/
    enum capture_options cap_opt;

    struct cim_ops ops;
};




#define CLEAR(x)	memset(&(x), 0, sizeof(x))
#if 0
struct buffer {
    void *	start;
    size_t	length;
};
#endif
static char *		dev_name	= NULL;
static int		fd		= -1;
struct Buffer *		buffers;
static unsigned int	n_buffers	= 0;
struct camera_info camera_inf;
static struct camera_ctl  camera_ctl;
struct v4l2_buffer buf;

static void errno_exit(const char *s)
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
    int r;

    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

static void cim_uninit()
{

    if(camera_ctl.io_method == IO_METHOD_USERPTR){
    }
    uninit_device(&camera_ctl, &camera_inf);	//cleanup video buf

    if (camera_inf.fd_cim > 0)
        close(camera_inf.fd_cim);

    if (camera_ctl.cam_opt == OPS_PREVIEW){

        if (camera_inf.ybuf) {
            free(camera_inf.ybuf);
            free(camera_inf.ubuf);
            free(camera_inf.vbuf);
        }

    }
}

static void do_clean()
{
    cim_uninit();
    exit(0);
}

static void sig_int(int signo)
{
    do_clean();
}
static void camera_fps(struct camera_info *camera_inf)
{

}

static int cim_init(int width, int height,__u32 fourcc)
{
    int bpp = 8;
    int ret = 0;
    camera_inf.param.bpp = bpp;
    camera_inf.fd_cim = -1;
    camera_inf.capture_limit = 5;

    camera_ctl.io_method = IO_METHOD_USERPTR;
    memset(camera_inf.file_name, 0, sizeof(camera_inf.file_name));
    memset(camera_inf.video_dev_name, 0, sizeof(camera_inf.video_dev_name));
    //snprintf(camera_inf.video_dev_name, 16, "/dev/video1");
    camera_inf.param.width = width;
    camera_inf.param.height =height;
    camera_inf.fmt.fourcc = fourcc;
    //camera_inf.fmt.format = YCbCr422_Y0CbY1Cr;
    //    camera_inf.fmt.fourcc =  V4L2_PIX_FMT_GREY;
    //   camera_inf.fmt.fourcc =0x2001 ;//V4L2_MBUS_FMT_Y8_1X8;



    camera_ctl.ops.priview_picture = NULL;//priview_picture;
    camera_ctl.ops.capture_picture = NULL;//capture_picture;
    camera_ctl.ops.camera_fps = camera_fps;

    camera_inf.jz_jpeg = NULL;//jz_jpeg_encode_init(width,height);

}
static void init_read(unsigned int buffer_size)
{
    buffers = calloc(1, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    buffers[0].length = buffer_size;
    buffers[0].start = malloc(buffer_size);

    if (!buffers[0].start) {
        fprintf(stderr, "Out of memory2\n");
        exit(EXIT_FAILURE);
    }
}
static void init_mmap(void)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count	= 5;
    req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory	= V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 1) {
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
        exit(EXIT_FAILURE);
    }

    buffers = malloc(req.count * sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers=0; n_buffers<req.count; ++n_buffers) {
       // struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory	= V4L2_MEMORY_MMAP;
        buf.index	= n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
                mmap(NULL, buf.length, PROT_READ, MAP_SHARED, fd, buf.m.offset);
              //  mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

static void init_userp(struct camera_info *camera_inf, unsigned int buffer_size)
{
    struct v4l2_requestbuffers req;
    unsigned int page_size = 0;
    int use_tlb = camera_inf->param.tlb;

    page_size = getpagesize();
    buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

    CLEAR(req);

    req.count	= 1;
    req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory	= V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support user pointer i/o\n", dev_name);
            exit(EXIT_FAILURE);
        } else
            errno_exit("VIDIOC_REQBUFS");
    }

    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        buffers[n_buffers].length = buffer_size;
        if (use_tlb)
            buffers[n_buffers].start = (unsigned char *)valloc(buffer_size);
        else
//            buffers[n_buffers].start = (unsigned char *)malloc(128, buffer_size);
            buffers[n_buffers].start = (unsigned char *)JZMalloc(128, buffer_size);

       // printf("==>%s L%d: the buffers[%d].start = %x\n", __func__, __LINE__ , n_buffers,buffers[n_buffers].start);
        if (!buffers[n_buffers].start) {
            fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

int open_device(const char *dev_name)
{
        struct stat st;

        if (-1 == stat(dev_name, &st)) {
                fprintf(stderr, "Cannot identify '%s':%d, %s\n", dev_name, errno, strerror(errno));
                //        exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is not char device\n", dev_name);
                //      exit(EXIT_FAILURE);
        }

    //	    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
        fd = open(dev_name, O_RDWR , 0);
        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s':%d, %s\n", dev_name, errno, strerror(errno));
                //    exit(EXIT_FAILURE);
        }

        return fd;
}

unsigned int set_format(struct camera_info *camera_inf)
{
    struct v4l2_format fmt;
    unsigned int min;
    int tem = 0;

    CLEAR(fmt);

    fmt.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("%dX%d\n",camera_inf->param.width,camera_inf->param.height);
    fmt.fmt.pix.width	= camera_inf->param.width;
    fmt.fmt.pix.height	= camera_inf->param.height;
    fmt.fmt.pix.pixelformat	= camera_inf->fmt.fourcc;
    fmt.fmt.pix.field	= V4L2_FIELD_ANY;/*V4L2_FIELD_NONE;*/
    //	fmt.fmt.pix.priv	= camera_inf->fmt.fmt_priv;

    if (-1 == ioctl(fd, VIDIOC_S_INPUT, &tem)) {
        fprintf(stderr, " VIDIOC_S_INPUT error\n");
        return -1;
    }

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
        printf("==>%s L%d: set error  !!!!!!!!!!!!! \n",__func__, __LINE__);
        errno_exit("VIDIOC_S_FMT");
    }

    if (-1 ==  xioctl(fd, VIDIOC_G_FMT, &fmt)){
        fprintf(stderr, " VIDIOC_G_FMT error\n");
        return -1;
    }
    if((camera_inf->param.width != fmt.fmt.pix.width) || (camera_inf->param.height != fmt.fmt.pix.height)){
        camera_inf->param.width = fmt.fmt.pix.width;
        camera_inf->param.height = fmt.fmt.pix.height;
    }

    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;

 /*   min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;
*/
    //printf("==>%s L%d: sizeimage=0x%x fmt.fmt.pix.width = 0x%x fmt.fmt.pix.height = 0x%x\n", __func__, __LINE__, fmt.fmt.pix.sizeimage,fmt.fmt.pix.width, fmt.fmt.pix.height);


    struct v4l2_streamparm Stream_Parm;
    memset(&Stream_Parm, 0, sizeof(struct v4l2_streamparm));
    Stream_Parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    Stream_Parm.parm.capture.timeperframe.denominator =60;
    Stream_Parm.parm.capture.timeperframe.numerator = 1;

    xioctl(fd, VIDIOC_S_PARM, &Stream_Parm);


    return fmt.fmt.pix.sizeimage;
}


void init_device(struct camera_info *camera_inf, struct camera_ctl *camera_ctl)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    enum io_method io = camera_ctl->io_method;
    unsigned int frmsize;
    int use_tlb = camera_inf->param.tlb;
    char *io_method_str[3] = {"IO_METHOD_READ", "IO_METHOD_MMAP", "IO_METHOD_USERPTR",};



    //printf("==>%s L%d: io method: %s\n", __func__, __LINE__, io_method_str[io]);

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is not V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        } else
            errno_exit("VIDIOC_QUERYCAP");
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is not capture device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    switch (io) {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            fprintf(stderr, "%s doesn't support read i/o\n", dev_name);
            exit(EXIT_FAILURE);
        }
        break;
    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            fprintf(stderr, "%s doesn't support streaming i/o\n", dev_name);
            exit(EXIT_FAILURE);
        }
        break;
    }

    /* select video input, video standard and tune here. */

    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;	/* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
            switch(errno) {
            case EINVAL:
                /* Cropping not supported */
                break;
            default:
                /* Errors ignored */
                break;
            }
        }
    } else {
        /* Errors ignored */
    }

    frmsize = set_format(camera_inf);

    switch (io) {
    case IO_METHOD_READ:
        init_read(frmsize);
        break;
    case IO_METHOD_MMAP:
        init_mmap();
        break;
    case IO_METHOD_USERPTR:
        init_userp(camera_inf, frmsize);
        break;
    }
}


void uninit_device(struct camera_ctl *camera_ctl, struct camera_info *camera_inf)
{
    unsigned int i;
    int use_tlb = camera_inf->param.tlb;
    enum io_method io = camera_ctl->io_method;

    switch(io) {
    case IO_METHOD_READ:
        free(buffers[0].start);
        break;
    case IO_METHOD_MMAP:
        for (i=0; i<n_buffers; ++i)
            if (-1 == munmap(buffers[i].start, buffers[i].length))
                errno_exit("munmap");
        break;
    case IO_METHOD_USERPTR:
        if (use_tlb)
            for (i=0; i<n_buffers; ++i)
                free(buffers[i].start);
        break;
    }

    free(buffers);
}



unsigned char *getyuv()
{

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = IO_METHOD_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
            case EAGAIN:
                return 0;
            case EIO:
            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }

	//clearyuv();

    return  (unsigned char*)buffers[buf.index].start;
}
unsigned char clearyuv()
{
    assert(buf.index < n_buffers);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");

    return 0;
}


void stop_capturing(struct camera_ctl *camera_ctl)
{
    enum v4l2_buf_type type;
    enum io_method io = camera_ctl->io_method;

    switch (io) {
    case IO_METHOD_READ:
        /* nothing to do */
        break;
    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF");

        break;
    }
}


void start_capturing(struct camera_ctl *camera_ctl)
{
    unsigned int i;
    enum v4l2_buf_type type;
    enum io_method io = camera_ctl->io_method;

    switch(io) {
    case IO_METHOD_READ:
        /* nothing to do */
        break;
    case IO_METHOD_MMAP:
        for (i=0; i<n_buffers; ++i) {
         //   struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");

        break;

    case IO_METHOD_USERPTR:
        for (i=0; i<n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long)buffers[i].start;
            buf.length = buffers[i].length;

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");

        break;
    }
}

int initCamera(int width, int height,__u32 fourcc)
{
    char buf[16];
    char *suffix = buf;
    int ret = 0;
    int squeue_count = 0;
  //  signal(SIGINT, sig_int);
    /*initialize cim device*/
    cim_init(width,height,fourcc);
    //camera_inf.fd_cim = open_device(camera_inf.video_dev_name);
    //if (camera_inf.fd_cim < 0) {
      //  printf("==>fail to open %s\n", camera_inf.video_dev_name);

        snprintf(camera_inf.video_dev_name, 16, "/dev/video0");
        camera_inf.fd_cim = open_device(camera_inf.video_dev_name);
        if (camera_inf.fd_cim < 0) {
            printf("==>fail to open %s\n", camera_inf.video_dev_name);
            return -1;
        }

    //}

    // camera_ctl.cap_opt = OPS_SAVE_RAW;
    //camera_ctl.io_method = IO_METHOD_USERPTR;

    /* initialize device*/
    init_device(&camera_inf, &camera_ctl);
    /* camera start capture*/
    start_capturing(&camera_ctl);
    /* process camera capture buf*/
    return ret;
}

int closecamera()
{

    /* camera stop capture*/
    stop_capturing(&camera_ctl);

    /* uninitalize cim device*/
    cim_uninit();

    printf("----------------------CIM TEST END -----------------------\n");


    return 0;
}
