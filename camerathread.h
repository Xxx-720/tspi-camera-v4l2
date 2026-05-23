#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <QDir>
#include <QDateTime>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    CameraThread();
    void stop();
    bool m_running = true;
    unsigned char* origin_data = nullptr;
signals:
    //帧准备好信号
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    void nv12_to_rgb(unsigned char* nv12, QImage &rgb);
};

class RecordThread : public QThread
{
    Q_OBJECT
public:
    RecordThread(CameraThread* camThread);
    bool m_recording = false;
    void stopRecord();
    void startRecord();

protected:
    void run() override;

private:
    QString dirpath =  "./img";


    CameraThread* m_camThread;
    bool m_record_running = false;

};









#endif // CAMERATHREAD_H
