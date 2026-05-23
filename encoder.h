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


#ifdef __cplusplus
}
#endif


void encode_init(char *name);
void encoder(unsigned char *buf);
void encode_close();





#endif // __ENCODER_H__
