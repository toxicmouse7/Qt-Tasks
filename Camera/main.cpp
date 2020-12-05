#include "mainwindow.h"
#include "audio.h"

#include <QApplication>
#include <QString>

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    IplImage* image = nullptr;

    image = cvLoadImage("image.png", CV_LOAD_IMAGE_GRAYSCALE);
    qDebug() << image->widthStep;


    //w.show();
    return a.exec();
}
