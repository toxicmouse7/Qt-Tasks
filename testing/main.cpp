#include "mainwindow.h"

#include <QApplication>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    for (const QCameraInfo& camerainfo : cameras)
    {
        w.AddText(camerainfo.deviceName());
    }

    w.show();
    return a.exec();
}
