#ifndef CAM_H
#define CAM_H

#include <QWidget>

#include <QCamera>
#include <QCameraImageCapture>
#include <QVideoWidget>
#include <QDateTime>
#include <QDir>
#include <QTimer>
#include <QVideoProbe>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QGraphicsVideoItem>

#include <camerathread.h>

QT_BEGIN_NAMESPACE
namespace Ui { class cam; }
QT_END_NAMESPACE

class cam : public QWidget
{
    Q_OBJECT

public:
    cam(QWidget *parent = nullptr);
    ~cam();



private slots:
    void save_pic(int id, const QImage &image);
    void album_init();
    void update_fps();
    void camera_restart();

    //拍照
    void on_take_photo_pressed();

    void on_take_video_pressed();

    void on_album_pressed();

private:
    Ui::cam *ui;

    QCameraImageCapture* imgCapture;    //拍照捕获
    QVideoWidget* viewfinder;            //
    QVideoProbe* videoProbe;
    QCamera* camera;    //相机设备

    QGraphicsView *videoView;
    QGraphicsScene *videoScene;
    QGraphicsVideoItem *videoItem;

    //路径是该项目下的路径
//    QString dirpath = QStringLiteral(PROJECT_SOURCE_DIR) + "/img";
    QString dirpath =  "./img";

    //FPS
    QTimer* fpstimer;
    int frame_count = 0;
    int fps;

    CameraThread* m_camThread;

};
#endif // CAM_H
