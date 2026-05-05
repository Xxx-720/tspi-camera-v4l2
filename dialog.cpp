#include "dialog.h"
#include "ui_dialog.h"

#include "cam.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    //创建垂直布局
    QVBoxLayout* v_layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    //设置为从上往下排
    v_layout->setAlignment(Qt::AlignTop);
    v_layout->setSpacing(30);

    if(!imgLabel)
    {
        imgLabel = new QLabel(ui->widget);
        //设置图片显示区域居中
        imgLabel->setAlignment(Qt::AlignCenter);
        imgLabel->setFixedSize(ui->widget->size());
        imgLabel->setStyleSheet("background:black");
    }

    //加载显示缩略图在左侧
    album_load_image();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::show_image()
{
    // qDebug() << "显示图片" << endl;

    // 获取发送信号的按钮
    QPushButton* new_btn = qobject_cast<QPushButton*>(sender());
    if(!new_btn)
    {
        return;
    }
    // 取消高亮
    if(btn && btn != new_btn)
    {
        btn->setStyleSheet("border:none");
    }

    btn = new_btn;
    // 选中高亮
    btn->setStyleSheet("border:2px solid #F7AFAF;");

    // 从按钮里取出我们存的图片路径
    QString path = btn->property("imagePath").toString();
    current_img_path = path;
    qDebug() << "current path:" << current_img_path << endl;

    QPixmap pixmap(path);
    if(pixmap.isNull())
    {
        qDebug() << "图片加载失败" << endl;
        return;
    }
    imgLabel->setPixmap(pixmap.scaled(ui->widget->size(),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
    
}

void Dialog::album_load_image()
{
    QDir dir(dirpath);
    if(dir.exists() == false)
    {
        dir.mkpath(".");
        qDebug() << "创建目录" << endl;
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

    //遍历所有图片
    for(QFileInfo info : list)
    {
        //获取每一个图片的路径
        QString image_path = info.absoluteFilePath();
        QPixmap pixmap(image_path);

        //缩略图
        QSize iconSize(120, 90);
        // 保持宽高比进行缩略
        QPixmap small_photo = pixmap.scaled(iconSize,
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);

        //创建图片按钮，按下就显示图片
        QPushButton* img_btn = new QPushButton(this->ui->scrollAreaWidgetContents);
        img_btn->setIcon(QIcon(small_photo));
        img_btn->setIconSize(iconSize);
        img_btn->setStyleSheet("border:none");
        img_btn->setProperty("imagePath", image_path);

        //将按钮加入垂直布局
        ui->scrollAreaWidgetContents->layout()->addWidget(img_btn);
        
        // 信号槽，连接按下与显示
        connect(img_btn, &QPushButton::clicked, this, &Dialog::show_image);
    }

}




void Dialog::on_del_clicked()
{
    if(current_img_path.isEmpty())
    {
        qDebug() << "还没选中图片" << endl;
        return;
    }
    //按下删除按钮删除该照片
    QFile file(current_img_path);
    if(file.exists() == false)
    {
        qDebug() << "文件不存在" << endl;
        return;
    }
    if(!file.remove())
    {
        qDebug() << "删除失败" << file.errorString() <<endl;
        return;
    }
    qDebug() << "删除成功" << current_img_path << endl;

    //清空显示图
    imgLabel->clear();
    current_img_path.clear();
    
    QLayout* layout = ui->scrollAreaWidgetContents->layout();
    //清空缩略图
    if(layout)
    {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr)
        {
            delete item->widget();
            delete item;
            qDebug() << "删除一个缩略图" << endl;
        }
    }
    btn = nullptr;

    // 重新加载缩略图
    album_load_image();
    
}

void Dialog::on_exit_clicked()
{
    qDebug() << "exit" << endl;
    //    delete imgLabel;

    this->close();
}
