#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void show_image();
    void album_load_image();


    void on_del_clicked();

    void on_exit_clicked();

private:
    Ui::Dialog *ui;

    //路径是该项目下的路径
//    QString dirpath = QStringLiteral(PROJECT_SOURCE_DIR) + "/img";
    QString dirpath =  "./img";
    QLabel* imgLabel = nullptr;
    QString current_img_path;
    QPushButton *btn = nullptr;
};

#endif // DIALOG_H
