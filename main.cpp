#include "cam.h"
#include "v4l2_app.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    cam w;

//    v4l2_start();


    w.show();
    return a.exec();
}
