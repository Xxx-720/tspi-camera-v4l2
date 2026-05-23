/*
    该例程是基于 V4L2 框架和 FFMPEG 的摄像头读取视频流并转成 MP4 文件保存,同时可以保存多个MP4文件,命名递增
    
    多线程：主线程->读取视频流  编码线程 -> 编码视频流
    编码器：h246_m2m (树莓派)
    摄像头抓取像素格式：NV12
    编码器像素格式：YUV420P
*/

#include "encoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VIDEO_WIDTH      640
#define VIDEO_HEIGHT     480

#define FPS              30


struct ffmpeg_ctx
{
    AVFormatContext *fmt_ctx;
    // AVStream *stream;
    AVCodecContext *codec_ctx;
    AVFrame *frame;
    AVPacket *pkt;
    struct SwsContext *sws_ctx;
    int pts;
};

struct ffmpeg_ctx prv_ctx;


void encode_init(char *name)
{
    printf("1. 编码器初始化\n");
    
    int ret;
    //找到编码器H.264
    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264); 
    // AVCodec* codec = avcodec_find_encoder_by_name(encoder_name); 
    if(!codec)
    {
        perror("    找不到对于编码器\n");
        return -1;
    }

    // 创建输出的上下文
    if((avformat_alloc_output_context2(prv_ctx.fmt_ctx, NULL, NULL, name)) < 0)
    {
        printf("    输出上下文创建失败\n");
    }
    //添加流 -> 流的 time_base 必须和编码器一致
    AVStream *video_stream = avformat_new_stream(prv_ctx.fmt_ctx, codec);
    if(video_stream == NULL)
    {
        printf("    创建视频流失败\n");
    }

    //创建编码器上下文，用于存放编码参数
    prv_ctx.codec_ctx = avcodec_alloc_context3(codec);
    if(!prv_ctx.codec_ctx)
    {
        perror("    allocate error");
        return -1;
    }

    //配置编码参数
    prv_ctx.codec_ctx->width = VIDEO_WIDTH;
    prv_ctx.codec_ctx->height = VIDEO_HEIGHT;
    prv_ctx.codec_ctx->time_base = (AVRational){1, FPS}; //时间帧
    prv_ctx.codec_ctx->framerate = (AVRational){FPS, 1}; //帧率
    prv_ctx.codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;       //像素格式 -> 决定了下面的原始帧的linesize的每一行数据字节大小
    prv_ctx.codec_ctx->gop_size = 10;                    //关键帧间隔
    prv_ctx.codec_ctx->max_b_frames = 0;                 //允许多少个B帧 //但是实时视频流不要B帧
    prv_ctx.codec_ctx->bit_rate = 800000;                //码率

    //打开编码器
    ret = avcodec_open2(prv_ctx.codec_ctx, codec, NULL);
    if(ret < 0)
    {
        perror("    编码器打开失败\n");
        return -1;
    }

    // 将编码参数复制到流中，配置输出格式
    ret = avcodec_parameters_from_context(video_stream->codecpar, prv_ctx.codec_ctx);
    video_stream->time_base = prv_ctx.codec_ctx->time_base;
    if(ret < 0)
    {
        printf("    复制编码参数到流中失败\n");

    }

    // 打开输出文件
    ret = avio_open(prv_ctx.fmt_ctx->pb, name, AVIO_FLAG_WRITE);
    if (ret < 0)
    {
        printf("    打开输出文件失败\n");

    }

    // 写入文件头
    ret = avformat_write_header(prv_ctx.fmt_ctx, NULL);
    if(ret < 0)
    {
        printf("    写入文件头失败\n");
    }

    //创建一个图像格式转换器
    prv_ctx.sws_ctx = sws_getContext(
        VIDEO_WIDTH, VIDEO_HEIGHT, AV_PIX_FMT_NV21,     //源
        VIDEO_WIDTH, VIDEO_HEIGHT, AV_PIX_FMT_YUV420P,  //目标
        SWS_BILINEAR, NULL, NULL, NULL);
    
    //创建原始帧
    prv_ctx.frame = av_frame_alloc();
    prv_ctx.frame->width = prv_ctx.codec_ctx->width; 
    prv_ctx.frame->height = prv_ctx.codec_ctx->height;
    prv_ctx.frame->format = prv_ctx.codec_ctx->pix_fmt;
    // // //少这一行会报错
    av_frame_get_buffer(prv_ctx.frame, 32);
    // frame->linesize[0] = codec_ctx->width; // Y平面  //设置步长，一行图像数据占多少字节，真实内存宽度，不是分辨率宽度
    // frame->linesize[1] = codec_ctx->width; // UV平面 //不同的格式会有不同的平面
    
    //创建编码数据包
    prv_ctx.pkt = av_packet_alloc();



}

/**
 * 视频编码函数
*/
void encoder(unsigned char *buf)
{
    printf("2. 编码一帧\n");
    
    int ret;
    
    /**
     * 规定:NV12中Y一个平面，UV一个平面
     * 其中，Y平面数据在最前面，UV紧跟其后
     * 所以，data[0]中存放Y平面数据，直接就是缓冲区地址；而data[1]是存放UV平面数据
     * 需要在起始地址加上Y平面的数据大小，Y平面总数据:宽*高
    */
    uint8_t *src_data[4] = {
                            buf,                             //Y平面起始地址
                            buf + VIDEO_WIDTH*VIDEO_HEIGHT,  //UV平面起始地址
                            NULL, NULL
    };
    int src_linesize[4] = { VIDEO_WIDTH,                        // Y 平面一行有多少字节
                            VIDEO_WIDTH,                        // UV平面一行有多少字节
                            0, 0 };

    sws_scale(prv_ctx.sws_ctx,
            src_data, src_linesize,
            0,                                                // 从第 0 行开始转换
            VIDEO_HEIGHT,                                     //转换全部行，即其高度
            prv_ctx.frame->data,                              //目标数据
            prv_ctx.frame->linesize);                         //目标行字节数


    //发送原始帧给编码器
    ret = avcodec_send_frame(prv_ctx.codec_ctx, prv_ctx.frame);
    if (ret < 0) {
        printf("   发送原始帧给编码器失败\n");
        exit(EXIT_FAILURE); 
    }

    prv_ctx.frame->pts = prv_ctx.pts++;
    
    while (ret >= 0) 
    {
        // 接受编码后的数据包
        ret = avcodec_receive_packet(prv_ctx.codec_ctx, prv_ctx.pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) 
        {
            printf("    接收数据包失败\n");
            exit(EXIT_FAILURE); 
        }
        /**
         * 得到编码后数据要进行的处理，如保存为文件，网络推流等
        */
        //将H.264数据写入文件
        // fwrite(pkt->data, 1, pkt->size, outfile);

        /**
         * packet 必须做时间基转换
        */
        av_packet_rescale_ts(prv_ctx.pkt, prv_ctx.codec_ctx->time_base, prv_ctx.fmt_ctx->streams[0]->time_base);
        
        // 将数据包写入文件
        ret = av_interleaved_write_frame(prv_ctx.fmt_ctx, prv_ctx.pkt);
        if(ret < 0)
        {
            printf("    数据写入文件失败/n");
        }

        // 释放包
        av_packet_unref(prv_ctx.pkt);
    }
    
}


void encode_close()
{
    printf("3. 编码结束\n");

    //冲刷编码器，洗掉剩余数据
    avcodec_send_frame(prv_ctx.codec_ctx, NULL);
    /**
     * 写入文件尾
    */
    av_write_trailer(prv_ctx.fmt_ctx);
    //释放资源
    // fclose(f);
    avio_context_free(prv_ctx.fmt_ctx->pb);
    avformat_free_context(prv_ctx.fmt_ctx);
    avcodec_free_context(&prv_ctx.codec_ctx);
    sws_freeContext(prv_ctx.sws_ctx);
    av_frame_free(&prv_ctx.frame);
    av_packet_free(&prv_ctx.pkt);

}

// int ffmpeg(const char* encoder_name, const char* output_name)
// {
//     printf("********ffmpeg的编码设置********\n")
//     int ret;

//     // //打开输出文件
//     // FILE* f = fopen(OUTPUT_FILE_NAME, "wb");



//     AVStream* video_stream;
//     AVFormatContext* fmt_ctx = NULL;
    
//     // mp4_init(&fmt_ctx, codec, codec_ctx, &video_stream, OUTPUT_FILE_NAME);
    
//     struct timespec start_time;
//     clock_gettime(CLOCK_REALTIME, &start_time);
//     int frame_count = 0;

//     int pts = 0;
//     printf("1. 开始编码\n");
//     /**
//      * 更改条件->触发录像
//     */
//     char newname[30];
//     // while(running)
//     // {
//     //     // 不拍 
//     //     if(!capture)
//     //     {
//     //         printf("/****** no ***********/\n");
//     //         usleep(50000);
//     //         //停止
//     //         if(stop_capture)
//     //         {
//     //             printf("/****** stop ***********/\n");
//     //             //冲刷编码器，洗掉剩余数据
//     //             encode(codec_ctx, NULL, pkt, fmt_ctx, &video_stream);

//     //             /**
//     //              * 写入文件尾
//     //             */
//     //             av_write_trailer(fmt_ctx);
//     //             //释放资源
//     //             // fclose(f);
//     //             avio_closep(&fmt_ctx->pb);
//     //             avformat_free_context(fmt_ctx);     

//     //             //插入链表
//     //             Node_insert_front(photo_video_list, newname);

//     //             stop_capture = 0;
//     //         }   
//     //         continue;
//     //     }
//     //     //新 - 拍
//     //     else if(capture && new_mp4_flag)
//     //     {
//     //         printf("/****** start turn ***********/\n");
//     //         count ++;
//     //         sprintf(newname, "%s/%d.mp4", SAVE_DIR_NAME, count);
//     //         printf("\n%s\n", newname);
//     //         // 初始化MP4
//     //         mp4_init(&fmt_ctx, codec, codec_ctx, &video_stream, newname);
//     //         //
//     //         // 2 = 0;
//     //         new_mp4_flag = 0;
//     //     } 


//     //     int found = -1;

//     //     for (size_t i = 0; i < REQ_BUFF_COUNT; i++)
//     //     {
//     //         pthread_mutex_lock(&buf[i].mutex);
//     //         if(buf[i].used == 1)
//     //         {
//     //             found = i;
//     //             // pthread_mutex_unlock(&buf[found].mutex);
//     //             break;               
//     //         }
//     //         pthread_mutex_unlock(&buf[i].mutex);
//     //     }
//     //     if (found == -1)
//     //     {
//     //         usleep(5000);
//     //         continue;
//     //     }
        

//     //     if (!running) 
//     //     {
//     //         pthread_mutex_unlock(&buf[found].mutex);
//     //         break;
//     //     }


            

//         //设置时间戳
//         // frame->pts = pts++;
//         struct timespec curruent_time;
//         clock_gettime(CLOCK_REALTIME, &curruent_time);
//         double elapsed_time = (curruent_time.tv_sec - start_time.tv_sec) +
//                             (curruent_time.tv_nsec - start_time.tv_nsec) / 1e9;
        
//         int64_t calculated_pts = (int64_t)(elapsed_time / av_q2d(codec_ctx->time_base));
//         frame->pts = calculated_pts;
        
//         //编码数据
//         encode(codec_ctx, frame, pkt, fmt_ctx, &video_stream);
        
//         buf[found].used = 0;
//         pthread_mutex_unlock(&buf[found].mutex);
//         pthread_cond_signal(&buf[found].cond);
        

//     printf("\n/*************mp4 is save now****************/\n");
// }

