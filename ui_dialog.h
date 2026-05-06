/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QWidget *widget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPushButton *del;
    QPushButton *exit;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(800, 480);
        widget = new QWidget(Dialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(160, 0, 640, 480));
        widget->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        scrollArea = new QScrollArea(Dialog);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(0, 49, 160, 431));
        scrollArea->setFocusPolicy(Qt::NoFocus);
        scrollArea->setStyleSheet(QString::fromUtf8("background-color: rgb(211, 215, 207);"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 144, 429));
        scrollArea->setWidget(scrollAreaWidgetContents);
        del = new QPushButton(Dialog);
        del->setObjectName(QString::fromUtf8("del"));
        del->setGeometry(QRect(80, 0, 80, 50));
        del->setStyleSheet(QString::fromUtf8("QPushButton:pressed\n"
"{\n"
"	background-color: rgb(255, 0, 0);\n"
"	border-radius: 20px;\n"
"}\n"
"QPushButton\n"
"{\n"
"	background-color: rgb(204, 0, 0);\n"
"	border-radius: 20px;\n"
"}\n"
""));
        exit = new QPushButton(Dialog);
        exit->setObjectName(QString::fromUtf8("exit"));
        exit->setGeometry(QRect(0, 0, 50, 50));
        QFont font;
        font.setPointSize(13);
        exit->setFont(font);
        exit->setStyleSheet(QString::fromUtf8("QPushButton\n"
"{\n"
"	background-color: rgb(136, 138, 133);\n"
"	border-radius: 20px;\n"
"}\n"
"QPushButton:pressed\n"
"{\n"
"	background-color: rgb(239, 41, 41);\n"
"}\n"
""));

        retranslateUi(Dialog);
        QObject::connect(exit, SIGNAL(clicked()), Dialog, SLOT(close()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        del->setText(QApplication::translate("Dialog", "Delete", nullptr));
        exit->setText(QApplication::translate("Dialog", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
