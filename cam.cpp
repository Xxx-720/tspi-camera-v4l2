#include "cam.h"
#include "ui_cam.h"
#include "dialog.h"
#include "camerathread.h"

#include <QDebug>



cam::cam(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::cam)
{
    ui->setupUi(this);
    // 无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);

    //相册初始化
    cam::album_init();
    
    m_camThread = new CameraThread();
    //启动线程
    m_camThread->start();


    //信号槽，连接拍照和保存
//    connect(imgCapture, &QCameraImageCapture::imageCaptured, this, &cam::save_pic);

    connect(m_camThread, &CameraThread::frameReady, this, [this](const QImage &img){
            QPixmap pixmap = QPixmap::fromImage(img);
            ui->label->setPixmap(pixmap.scaled(ui->label->size(),
                      Qt::KeepAspectRatio,
                      Qt::SmoothTransformation));
            frame_count ++;
    });

    qDebug() << "启动成功" << endl;
    

    fpstimer = new QTimer(this);
    // 一秒更新一次
    fpstimer->setInterval(1000);
    fpstimer->start();

    //信号槽，连接定时器和FPS的更新
    connect(fpstimer, &QTimer::timeout, this, &cam::update_fps);

    // qDebug() << "Timer started"<< endl;
}

cam::~cam()
{
    delete ui;
}

void cam::album_init()
{
    QDir dir(dirpath);
    if(dir.exists() == false)
    {
        dir.mkpath(".");
        qDebug() << "创建目录" << endl;
        return;
    }

    //获取目录下的所有图片
    QStringList filters;
    // 只要这些格式
    filters << "*.jpg" << "*.jpeg" << "*.png" ;
    dir.setNameFilters(filters);
    //时间排序
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList(QDir::Files);
    //如果没有图片则不显示在缩略图中
    if(list.isEmpty())
    {
        qDebug() << "404 Not Found" << endl;
        return;
    }
    //最新的图片
    QFileInfo new_file = list.first();
    QString image_path = new_file.absoluteFilePath();
    // qDebug() << "path:" << image_path << endl;

    ui->album->setIcon(QIcon(image_path));

}

void cam::save_pic(int id, const QImage &image)
{
    // 创建一个不可变的字符串，保存图片的目录
//    const QString dir_path = QStringLiteral("./img");

    // 创建一个不可变的字符串，保存图片的格式
    const QString end_name = QStringLiteral("jpg");

    QDir dir(dirpath);
    if(dir.exists() == false)
    {
        dir.mkpath(".");
        qDebug() << "创建目录" << endl;
    }
    // qDebug() << "目录真实绝对路径：" << dir.absolutePath();
    
    //只用秒会出现快拍覆盖的情况
    QString save_name = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + "." + end_name;
    QString save = dir.filePath(save_name);
    
    //保存图片
    //必须加上格式
    bool success = image.save(save, end_name.toUtf8().constData());
    if (!success)
    {
        qDebug() << "保存图片失败" << endl;
        return;
    }

    //1 saved to "./img/20260419_145524.jpg"
    qDebug() << id << "saved to" << save << endl;

    //缩略图
    QSize iconSize(120, 90);
    // 保持宽高比进行缩略
    QPixmap small_photo = QPixmap::fromImage(image).scaled(iconSize,
                                                            Qt::KeepAspectRatio, 
                                                            Qt::SmoothTransformation);
    ui->album->setIcon(QIcon(small_photo));    
//    qDebug() << "大小: " << ui->album->size() << endl;
}

void cam::update_fps()
{
    fps = frame_count;
//    qDebug() << "FPS:" << fps << endl;
    ui->FPS->setText(QString("FPS: %1").arg(fps));
    frame_count = 0;
}


void cam::on_take_photo_pressed()
{
//    if(camera->state() != QCamera::ActiveState) {
//        qDebug() << "相机未启动，无法拍照" << endl;
//        return;
//    }
//    //触发拍照
//    imgCapture->capture();
    qDebug() << "take a photo" << endl;
}

void cam::on_take_video_pressed()
{

}


void cam::camera_restart()
{
//    if(!camera)
//    {
//        camera = new QCamera(this);
//    }

//    cam::album_init();
////    if(!viewfinder)
////    {
////        viewfinder = new QVideoWidget(ui->widget);
////    }
////    viewfinder->show();
////    camera->setViewfinder(viewfinder);   // 重新绑定显示窗口

//    camera->start();

}

void cam::on_album_pressed()
{
    Dialog* d = new Dialog(this);
    
    // 打开相册窗口时先停止相机，关闭预览
//    camera->stop();
//    viewfinder->hide();

    // 关闭相册后重新启动相机
    connect(d, &Dialog::finished, this, &cam::camera_restart);

    d->setParent(this);
    d->move(0, 0);        // 固定在左上角
    d->resize(size());

//    d->show();
    d->showFullScreen();
    // camera->stop();
    // viewfinder->close();

    // delete camera;
    // delete imgCapture;
    // delete viewfinder;
}
