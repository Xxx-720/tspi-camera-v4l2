#include "camerathread.h"
#include "v4l2_app.h"
#include "encoder.h"

#include <rga/RgaApi.h>
#include <rga/im2d.h>

#define IMG_WIDTH 640
#define IMG_HIGHT 480

CameraThread::CameraThread()
{

}

void CameraThread::stop()
{
    m_running = false;

    //等待线程退出
    wait();

    v4l2_stop();


}

bool nv12_to_rgb(unsigned char* nv12, QImage &rgb)
{
//    //Y分量的起始地址
//    uint8_t* y = nv12;
//    //VU分量的起始地址
//    uint8_t* vu = nv12 + IMG_HIGHT*IMG_WIDTH;
//    uint8_t* dst = rgb.bits();

//    //每一行
//    for(int i=0; i<IMG_HIGHT; i++)
//    {
//        //每一列
//        for(int j=0; j<IMG_WIDTH; j++)
//        {
//            //现在是第几行的哪个Y分量
//            int yy = y[i*IMG_WIDTH + j];
//            //U分量
//            int u = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2] - 128;
//            //V分量
//            int v = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2 + 1] - 128;

//            //公式
//            int r = yy + 1.370705f * v;
//            int g = yy - 0.698001f * v - 0.337633f * u;
//            int b = yy + 1.732446f * u;

//            r = qBound(0, r, 255);
//            g = qBound(0, g, 255);
//            b = qBound(0, b, 255);

//            *dst++ = r;
//            *dst++ = g;
//            *dst++ = b;

//        }
//    }


    // 老版驱动直接用虚拟地址包装，不导入 handle
    rga_buffer_t src = wrapbuffer_virtualaddr(nv12, 640, 480,
                                               RK_FORMAT_YCbCr_420_SP,
                                               640,   // wstride
                                               480);  // hstride
    unsigned char* rgb_buf =rgb.bits();
    rga_buffer_t dst = wrapbuffer_virtualaddr(rgb_buf, 640, 480,
                                               RK_FORMAT_RGB_888,
                                               640,  // wstride: RGB888 = width*3
                                               480);

    // 老版可能没有 imcheck，直接调用
    IM_STATUS ret = imcvtcolor(src, dst,
                               RK_FORMAT_YCbCr_420_SP,
                               RK_FORMAT_RGB_888,
                               IM_YUV_TO_RGB_BT601_LIMIT);

    if (ret != IM_STATUS_SUCCESS) {
//        qDebug() << "RGA convert failed:" << imStrError(ret);
        return false;
    }
    return true;

}
//void CameraThread::nv12_to_rgb(unsigned char* nv12, QImage &rgb)
//{
//    //Y分量的起始地址
//    uint8_t* y = nv12;
//    //VU分量的起始地址
//    uint8_t* vu = nv12 + IMG_HIGHT*IMG_WIDTH;
//    uint8_t* dst = rgb.bits();

//    //每一行
//    for(int i=0; i<IMG_HIGHT; i++)
//    {
//        //每一列
//        for(int j=0; j<IMG_WIDTH; j++)
//        {
//            //现在是第几行的哪个Y分量
//            int yy = y[i*IMG_WIDTH + j];
//            //U分量
//            int u = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2] - 128;
//            //V分量
//            int v = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2 + 1] - 128;

//            //公式
//            int r = yy + 1.370705f * v;
//            int g = yy - 0.698001f * v - 0.337633f * u;
//            int b = yy + 1.732446f * u;

//            r = qBound(0, r, 255);
//            g = qBound(0, g, 255);
//            b = qBound(0, b, 255);

//            *dst++ = r;
//            *dst++ = g;
//            *dst++ = b;

//        }
//    }

//}

void CameraThread::run()
{
    m_running = true;

    //启动v4l2
    v4l2_start();

    printf("********CameraThread线程启动********\n");

    QImage frame(IMG_WIDTH, IMG_HIGHT, QImage::Format_RGB888);

    printf("    抓取中...");
    while(m_running)
    {
        //捕获
        capturing(&origin_data);
        //转换格式
        nv12_to_rgb(origin_data, frame);
        //发送信号给UI
        emit frameReady(frame);
//        printf(".");
    }
    printf("\n========CameraThrea线程结束========\n");
}



/* ============ RecordThread================== */
/* =========================================== */

RecordThread::RecordThread(CameraThread* camThread)
{
    m_camThread = camThread;
}

void RecordThread::startRecord()
{
    m_record_running = true;
    m_recording = true;
    start();
}

void RecordThread::stopRecord()
{
    m_record_running = false;
    m_recording = false;
    wait();
    encode_close();
}

void RecordThread::run()
{
    printf("********RecordThread线程启动********\n");

    QDir dir(dirpath);
    if(dir.exists() == false)
    {
        dir.mkpath(".");
        printf("创建目录\n");
    }

//    show_encoder();

    //在预览
    while(m_record_running)
    {
        if(!m_camThread->m_running)
        {
            break;
        }

        //开始录像
        if(!m_recording)
        {
            msleep(10);
            continue;
        }
        printf("    开始录像\n");
        if(prv_ctx.fmt_ctx == NULL)
        {
            const QString end_name = QStringLiteral("mp4");

            QString save_name = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + "." + end_name;
            QString save = dir.filePath(save_name);
//            printf("视频保存到%s\n", &save);

            encode_init(save.toLocal8Bit().data());
        }
//        unsigned char *data = nullptr;
//        capturing(&data);
        encoder(m_camThread->origin_data);

//        msleep(33);

    }

    printf("\n********RecordThread线程结束********\n");
}



/* ============ PlayThread================== */
/* =========================================== */
VideoPlayThread::VideoPlayThread(CameraThread* camThread)
{
    m_camThread = camThread;
}

void VideoPlayThread::startPlay(const QString &filePath)
{
    m_playing = true;
    m_filePath = filePath;

    start();
}


void VideoPlayThread::stopPlay()
{
    m_playing = false;
    wait();
//    decode_close(decoder_ctx);
    if(mp4_data)
    {
        free(mp4_data);
        mp4_data = nullptr;
    }

}

QImage VideoPlayThread::albumFirstFrame(QString path)
{
    QImage FirstFrame(IMG_WIDTH, IMG_HIGHT, QImage::Format_RGB888);
    unsigned char* data = nullptr;

    struct ffmpeg_ctx* tmp_ctx = decoder_init(path.toLocal8Bit().data());

    printf("decoder_init结束，decoder开始\n");

    decoder(1, &data, tmp_ctx);
    if(data)
    {
        printf("    nv12转rgb\n");
        nv12_to_rgb(data, FirstFrame);
        free(data);
    }
    printf("    nv12结束\n");
    decode_close(tmp_ctx);
    printf("    albumFirstFrame结束\n");

    return FirstFrame;
}


void VideoPlayThread::run()
{
    printf("********VideoPlayThread线程启动********\n");

    decoder_ctx = decoder_init(m_filePath.toLocal8Bit().data());

    QImage frame(IMG_WIDTH, IMG_HIGHT, QImage::Format_RGB888);

    printf("    开始播放\n");

    while(m_playing)
    {
        decoder(0, &mp4_data, decoder_ctx);
        if(mp4_data == NULL)
        {
//            avcodec_send_packet(decoder_ctx->codec_ctx, NULL);
//            AVFrame* tmp_frame = av_frame_alloc();
//            while (avcodec_receive_frame(decoder_ctx->codec_ctx, tmp_frame) == 0) {}
//            av_frame_free(&tmp_frame);

            printf("    最后一帧\n");
            m_playing = false;
            break;
        }
        nv12_to_rgb(mp4_data, frame);
        free(mp4_data);
        mp4_data = nullptr;
        emit playReady(frame);

//        QThread::msleep(1);
    }

    decode_close(decoder_ctx);

    printf("********VideoPlayThread线程结束********\n");
}




