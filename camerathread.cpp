#include "camerathread.h"
#include "v4l2_app.h"


#define IMG_WIDTH 1920
#define IMG_HIGHT 1080

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

void CameraThread::nv21_to_rgb(unsigned char* nv12, QImage &rgb)
{
    //Y分量的起始地址
    uint8_t* y = nv12;
    //VU分量的起始地址
    uint8_t* vu = nv12 + IMG_HIGHT*IMG_WIDTH;
    uint8_t* dst = rgb.bits();

    //每一行
    for(int i=0; i<IMG_HIGHT; i++)
    {
        //每一列
        for(int j=0; j<IMG_WIDTH; j++)
        {
            //现在是第几行的哪个Y分量
            int yy = y[i*IMG_WIDTH + j];
            //V分量
            int v = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2] - 128;
            //U分量
            int u = vu[(i/2)*(IMG_WIDTH/2)*2 + (j/2)*2 + 1] - 128;

            //公式
            int r = yy + 1.370705f * v;
            int g = yy - 0.698001f * v - 0.337633f * u;
            int b = yy + 1.732446f * u;

            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);

            *dst++ = r;
            *dst++ = g;
            *dst++ = b;

        }
    }

}

void CameraThread::run()
{
    m_running = true;

    //启动v4l2
    v4l2_start();

    printf("********CameraThread线程启动********\n");

    QImage frame(IMG_WIDTH, IMG_HIGHT, QImage::Format_RGB888);
    unsigned char* origin_data = nullptr;

    printf("    抓取中...");
    while(m_running)
    {
        //捕获
        capturing(&origin_data);
        //转换格式
        nv21_to_rgb(origin_data, frame);
        //发送信号给UI
        emit frameReady(frame);
//        printf(".");
    }
    printf("\n========CameraThrea线程结束========\n");
}
