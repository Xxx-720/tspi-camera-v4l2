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

//    void nv12_to_rgb(unsigned char* nv12, QImage &rgb);
signals:
    //帧准备好信号
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:

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



class VideoPlayThread : public QThread
{
    Q_OBJECT
public:
    VideoPlayThread(CameraThread* camThread);
    void startPlay(const QString & filePath);
    void stopPlay();
    static QImage albumFirstFrame(QString path);

protected:
    void run() override;


private:
    bool m_playing = false;

    QString m_filePath;

    unsigned char* mp4_data = nullptr;


    CameraThread* m_camThread;

    struct ffmpeg_ctx* decoder_ctx;

    bool video_album_save = false;
signals:
    void playReady(const QImage &frame);

};







#endif // CAMERATHREAD_H
