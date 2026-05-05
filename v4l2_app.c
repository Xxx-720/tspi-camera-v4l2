/*
    该例程是基于 V4L2 框架的多平面摄像头拍照

    摄像头抓取像素格式：NV12
*/

#include <stdio.h>
#include "v4l2_app.h"

#define DEVICE_NAME "/dev/video0"
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define REQ_BUFF_COUNT 4
#define MAX_PLANES 8

// 缓冲区结构体
struct buffer
{
    struct v4l2_plane *plane; // 平面
    int n_planes;             // 平面数
    void **start;             // 缓冲区起始地址
    size_t *length;           // 缓冲区长度
};
struct buffer *buffers = NULL;

static int video_fd = -1;

void request_buffers()
{
    printf("3. 申请视频缓冲区...\n");

    // 3.申请内核空间(只是申请了，但是还没有映射)
    struct v4l2_requestbuffers req_buff;
    // V4L2_BUF_TYPE_VIDEO_CAPTURE -> 单平面
    // V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE -> 多平面
    req_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    req_buff.count = REQ_BUFF_COUNT;    // 申请4个缓冲区（最大4个）
    req_buff.memory = V4L2_MEMORY_MMAP; // 使用映射的方式
    // VIDIOC_REQBUFS 申请内存空间

    if ((ioctl(video_fd, VIDIOC_REQBUFS, &req_buff)) < 0)
    {
        perror("request buff ioctl error");
        close(video_fd);
        exit(EXIT_FAILURE);
    }

    printf("    分配了%d个缓存区\n", req_buff.count);

    // 分配缓冲区
    buffers = calloc(req_buff.count, sizeof(struct buffer));
    if (!buffers)
    {
        printf("分配缓冲区失败\n");
        close(video_fd);
        exit(EXIT_FAILURE);
    }
}

void map_buffers()
{
    printf("4. 映射缓冲区...\n");

    // 4.映射
    int ret;
    // 每一个缓冲区都要映射
    for (size_t i = 0; i < REQ_BUFF_COUNT; i++)
    {
        struct v4l2_buffer buf = {0};
        struct v4l2_plane planes[MAX_PLANES] = {0};

        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.length = MAX_PLANES; // 平面
        buf.m.planes = planes;   // 平面数组

        // VIDIOC_QUERYBUF 查询内核给你的视频缓冲区详情
        ret = ioctl(video_fd, VIDIOC_QUERYBUF, &buf);
        if (ret < 0)
        {
            perror("Query buff ioctl error");
            close(video_fd);
            exit(EXIT_FAILURE);
        }

        // 为每一个缓冲区分配内存空间
        buffers[i].plane = calloc(buf.length, sizeof(struct v4l2_plane));
        buffers[i].n_planes = buf.length;
        buffers[i].start = malloc(buf.length * sizeof(void *));
        buffers[i].length = malloc(buf.length * sizeof(size_t));

        // 为每一个缓冲区的每一个平面映射
        for (int j = 0; j < buf.length; j++)
        {
            // 缓冲区的每一个平面
            buffers[i].plane[j] = planes[j];
            buffers[i].length[j] = planes[j].length;
            // 映射
            buffers[i].start[j] = mmap(NULL, planes[j].length,
                                       PROT_READ | PROT_WRITE, MAP_SHARED,
                                       video_fd, planes[j].m.mem_offset);
            if (buffers[i].start[j] == MAP_FAILED)
            {
                printf("    映射失败\n");
                close(video_fd);
                exit(EXIT_FAILURE);
            }
            printf("    映射成功，地址：%p\n", buffers[i].start[j]);
        }
        // 映射好后要将缓冲区放入队列，这样内核才能将数据填写如缓冲区
        //  VIDIOC_QBUF 入队,把空缓冲区交给内核，等待摄像头填数据
        //  ret = ioctl(video_fd, VIDIOC_QBUF, &v4buf);
        //  if (ret < 0)
        //  {
        //      perror("qbuf ioctl error");
        //      close(video_fd);
        //  }
    }

    printf("    所有映射区完成\n");
}

void queue_buffers()
{
    //5.将缓冲区放入队列
    printf("5. 将缓冲区放入队列...\n");

    for(size_t i=0; i<REQ_BUFF_COUNT; i++)
    {
        struct v4l2_buffer buf = {0};
        struct v4l2_plane planes[MAX_PLANES] = {0};

        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.length = MAX_PLANES; // 平面
        buf.m.planes = planes;   // 平面数组

        // VIDIOC_QBUF 入队,把空缓冲区交给内核，等待摄像头填数据
        if((ioctl(video_fd, VIDIOC_QBUF, &buf)) < 0)
        {
            perror("qbuf ioctl error");
        } 

        printf("    缓冲区%ld放入队列成功\n", i);
    }
    printf("    所有缓冲区放入队列成功\n");
}

void start_stream()
{
    // 6.开始视频流
    printf("6. 开始视频流...\n");
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if ((ioctl(video_fd, VIDIOC_STREAMON, &type)) < 0)
    {
        perror("stream on ioctl error");
        close(video_fd);
        exit(EXIT_FAILURE);
    }
}

void save_frame(int index, int length)
{
    //保存图片
    static int i = 0; 
    char name[30];
    sprintf(name, "frame%d.yuv", i++); 
    FILE* file = fopen(name, "wb");

    fwrite(buffers[index].start[0], 1, length, file);
    fclose(file);

    printf("    成功保存%s\n", name);
}

void capturing(unsigned char** origin_data)
{
    //7.开始采集
//    printf("7. 开始采集...\n");

    //从队列中取一帧数据（出队）
    struct v4l2_buffer v4read_buf = {0}; //存放读取的数据
    struct v4l2_plane planes[MAX_PLANES] = {0};

    v4read_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    v4read_buf.memory = V4L2_MEMORY_MMAP;
    v4read_buf.length = MAX_PLANES;
    v4read_buf.m.planes = planes;

//    while(1)
//    {
    if((ioctl(video_fd, VIDIOC_DQBUF, &v4read_buf)) < 0)
    {
        perror("Dqbuf ioctl error");
        close(video_fd);
    }

    // save_frame(v4read_buf.index, v4read_buf.m.planes[0].bytesused);
    *origin_data = buffers[v4read_buf.index].start[0];

    //该帧使用完成，重新入队

    if((ioctl(video_fd, VIDIOC_QBUF, &v4read_buf)) < 0)
    {
        perror("qbuf2 ioctl error");
        close(video_fd);
    }
//    }


}


void v4l2_start()
{
    printf("*******V4L2的多平面缓冲区设置********\n");

    // 1.打开设备
    printf("1. 打开设备...\n");
    video_fd = open(DEVICE_NAME, O_RDWR);
    if (video_fd < 0)
    {
        perror("video open");
//        exit(EXIT_FAILURE);
        return;
    }
    printf("    设备打开成功，文件描述符：%d\n", video_fd);

    // 2.设置采集格式
    printf("2. 设置采集格式...\n");
    struct v4l2_format v4fmt = {0};
    // 设置数据流类型，即告诉内核我要操作的功能是什么
    v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    // 设置摄像头采集的参数
    // 注意这里使用的是多平面格式
    v4fmt.fmt.pix_mp.width = VIDEO_WIDTH;             // 宽
    v4fmt.fmt.pix_mp.height = VIDEO_HEIGHT;           // 高
    v4fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV21; // 设置视频采集格式 V4L2_PIX_FMT_MJPEG
    // v4fmt.fmt.pix_mp.num_planes = 2;                  // 设置平面数
    // v4fmt.fmt.pix_mp.field = V4L2_FIELD_NONE; //设置采集方式
    // VIDIOC_S_FMT 设置格式
    if ((ioctl(video_fd, VIDIOC_S_FMT, &v4fmt)) < 0)
    {
        perror("set format ioctl error");
        close(video_fd);
        exit(EXIT_FAILURE);
    }

    printf("    设置采集格式成功 %d*%d\n", v4fmt.fmt.pix_mp.width, v4fmt.fmt.pix_mp.height);

    request_buffers();

    map_buffers();

    //
    queue_buffers();

    start_stream();

//    capturing();

    // //6.停止采集
    // ioctl(video_fd, VIDIOC_STREAMOFF, &type);

    // //释放映射
    // for (size_t i = 0; i < REQ_BUFF_COUNT; i++)
    // {
    //     munmap(map_buf_start[i], length[i]);
    // }

    // 9.关闭设备
//    close(video_fd);

    printf("========V4L2的多平面缓冲区设置========\n");
}
