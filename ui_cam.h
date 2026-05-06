/********************************************************************************
** Form generated from reading UI file 'cam.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAM_H
#define UI_CAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cam
{
public:
    QPushButton *take_photo;
    QPushButton *album;
    QPushButton *take_video;
    QLabel *FPS;
    QLabel *label;

    void setupUi(QWidget *cam)
    {
        if (cam->objectName().isEmpty())
            cam->setObjectName(QString::fromUtf8("cam"));
        cam->resize(800, 480);
        cam->setStyleSheet(QString::fromUtf8("background-color: rgb(245, 121, 0);"));
        take_photo = new QPushButton(cam);
        take_photo->setObjectName(QString::fromUtf8("take_photo"));
        take_photo->setGeometry(QRect(680, 200, 80, 80));
        QFont font;
        font.setPointSize(13);
        font.setBold(false);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(50);
        take_photo->setFont(font);
        take_photo->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"border-radius: 40px;"));
        album = new QPushButton(cam);
        album->setObjectName(QString::fromUtf8("album"));
        album->setGeometry(QRect(660, 360, 120, 90));
        album->setIconSize(QSize(120, 90));
        take_video = new QPushButton(cam);
        take_video->setObjectName(QString::fromUtf8("take_video"));
        take_video->setGeometry(QRect(680, 60, 80, 80));
        take_video->setFont(font);
        take_video->setStyleSheet(QString::fromUtf8("background-color:rgb(255, 0, 0);\n"
"border-radius: 40px;\n"
""));
        FPS = new QLabel(cam);
        FPS->setObjectName(QString::fromUtf8("FPS"));
        FPS->setGeometry(QRect(650, 10, 91, 40));
        QFont font1;
        font1.setPointSize(15);
        font1.setBold(true);
        font1.setWeight(75);
        FPS->setFont(font1);
        FPS->setStyleSheet(QString::fromUtf8("background-color: transparent;\n"
"color: rgb(255, 255, 255);"));
        label = new QLabel(cam);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 640, 480));
        label->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));

        retranslateUi(cam);

        QMetaObject::connectSlotsByName(cam);
    } // setupUi

    void retranslateUi(QWidget *cam)
    {
        cam->setWindowTitle(QApplication::translate("cam", "cam", nullptr));
        take_photo->setText(QApplication::translate("cam", "\346\213\215\347\205\247", nullptr));
        album->setText(QString());
        take_video->setText(QApplication::translate("cam", "\345\275\225\345\203\217", nullptr));
        FPS->setText(QString());
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class cam: public Ui_cam {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAM_H
