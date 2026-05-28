#ifndef __ENCODER_H__
#define __ENCODER_H__


#ifdef __cplusplus
extern "C" {
#endif


//视频流头文件
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>


struct ffmpeg_ctx
{
    AVFormatContext *fmt_ctx;
    // AVStream *stream;
    AVCodecContext *codec_ctx;
    AVFrame *frame;
    AVPacket *pkt;
    struct SwsContext *sws_ctx;

    int64_t start_time;
    int pts;
};

extern struct ffmpeg_ctx prv_ctx;

void show_encoder();
void encode_init(char *name);
void encoder(unsigned char *buf);
void encode_close();

#ifdef __cplusplus
}
#endif









#endif // __ENCODER_H__
