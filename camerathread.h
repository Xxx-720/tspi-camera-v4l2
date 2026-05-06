#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>


class CameraThread : public QThread
{
    Q_OBJECT
public:
    CameraThread();
    void stop();

signals:
    //帧准备好信号
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    bool m_running = true;
    void nv21_to_rgb(unsigned char* nv12, QImage &rgb);
};

#endif // CAMERATHREAD_H
