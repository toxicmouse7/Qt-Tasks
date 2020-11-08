#include "camera.h"

Camera::Camera(QWidget* CameraPlacement)
{
    camera_placement = CameraPlacement;
    availableCameras = QCameraInfo::availableCameras();
    timer.reset(new QTimer);
    timer->setInterval(5);
    connect(timer.data(), SIGNAL(timeout()), this, SLOT(cvRecord()));

    ConnectCameraStatus();
}

QList<QCameraInfo> Camera::GetAvailableCameras()
{
    return QCameraInfo::availableCameras();
}

void Camera::UpdateDevices()
{
    availableCameras.clear();

    emit ClearComboBox();

    availableCameras = QCameraInfo::availableCameras();

    emit UpdateComboBox(availableCameras);
}

void Camera::LoadCamera()
{
    curCameraIndex = 0;

    curCamera.reset(new QCamera(availableCameras[0]));
    curCamera->load();

    ConnectCameraStatus();

    resolutions = curCamera->supportedViewfinderResolutions();

    curCameraViewFinder.reset(new QCameraViewfinder(camera_placement));
    cameraFocus = curCamera->focus();

    imageCapture.reset(new QCameraImageCapture(curCamera.data()));
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);

    curCameraViewFinder->setFixedSize(QSize(camera_placement->width(), camera_placement->height()));

    curCamera->setViewfinder(curCameraViewFinder.data());
    curCamera->setCaptureMode(QCamera::CaptureVideo);

    curCameraViewFinder->show();
    curCamera->start();
}

void Camera::LoadCamera(int index)
{
    curCameraIndex = index;

    curCamera.reset(new QCamera(availableCameras[index]));
    curCamera->load();

    ConnectCameraStatus();

    resolutions = curCamera->supportedViewfinderResolutions();

    curCameraViewFinder.reset(new QCameraViewfinder(camera_placement));
    cameraFocus = curCamera->focus();

    imageCapture.reset(new QCameraImageCapture(curCamera.data()));
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);

    curCameraViewFinder->setFixedSize(QSize(camera_placement->width(), camera_placement->height()));

    curCamera->setViewfinder(curCameraViewFinder.data());
    curCamera->setCaptureMode(QCamera::CaptureVideo);

    curCameraViewFinder->show();
    curCamera->start();
}

void Camera::ChangeCamera(int index)
{
    if (index >= 0)
    {
        if (!curCamera.isNull())
        {
            curCamera.reset();
            curCameraViewFinder.reset();
            imageCapture.reset();
            resolutions.clear();
        }

        if (index < availableCameras.size())
        {
            LoadCamera(index);
        }
    }
}

void Camera::CamError(QCamera::Status status)
{
    QMessageBox msgBox;
    switch (status)
    {
        case QCamera::Status::UnavailableStatus:
        {
            msgBox.setText("Camera status: Unavailable");
            msgBox.exec();
            break;
        }

        case QCamera::Status::StandbyStatus:
        {
            msgBox.setText("Camera status: Standby");
            msgBox.exec();
            break;
        }

        case QCamera::Status::StoppingStatus:
        {
            msgBox.setText("Camera status: Stopping");
            msgBox.exec();
            break;
        }
    }
}

void Camera::StartRecord(QString& filename)
{
    QObject::disconnect(curCamera.data(), SIGNAL(statusChanged(QCamera::Status)), this, SLOT(CamError(QCamera::Status)));
    curCamera->unload();
    curCamera->stop();
    ConnectCameraStatus();

    cv::Mat src;
    recorder.open(curCameraIndex);

    recorder >> src;

    int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');

    double fps = 30;
    /*cv::Size size(resolutions[resolutions.size() - 1].width(), resolutions[resolutions.size() - 1].height());

    if (size.width > 1920)
    {
        size.width = resolutions[0].width();
    }
    if (size.height > 1080)
    {
        size.height = resolutions[0].height();
    }*/

    //cv::VideoWriter _recorder("output " + filename.toStdString() + ".avi", CV_FOURCC('X','V','I','D'), fps, size);
    writer.reset(new cv::VideoWriter("output " + filename.toStdString() + ".avi", codec, fps, src.size()));

    cv::namedWindow("Live");

    timer->start();
    /*while (recorder.isOpened() && recorder.read(frame))
    {
        if (cv::getWindowProperty("Record", cv::WND_PROP_VISIBLE) <= 0)
        {
            cv::destroyAllWindows();
            emit StopAllRecords(filename);
            break;
        }

        _recorder.write(frame);
        //QImage image = MatToQImage(frame);
        //ShowImage(label.data(), &image);
        cv::imshow("Record", frame);
        cv::waitKey(10);
    }*/

    /*recorder.release();
    _recorder.release();

    curCamera->load();
    curCamera->start();
    ConnectCameraStatus();*/
}

void Camera::StopRecord()
{
    timer->stop();
    recorder.release();
    writer->release();
    writer.reset();

    curCamera->load();
    curCamera->start();
    //ConnectCameraStatus();
}

void Camera::StandartLoad()
{
    if (availableCameras.size() > 0)
    {
        LoadCamera();
    }
    else
    {
        QMessageBox msg;
        msg.setText("No cameras detected");
        msg.exec();
    }
}

void Camera::cvRecord()
{
    cv::Mat frame;
    if (recorder.isOpened() && recorder.read(frame))
    {
        writer->write(frame);
        cv::imshow("Live", frame);
    }
}

void Camera::ConnectCameraStatus()
{
    QObject::connect(curCamera.data(), SIGNAL(statusChanged(QCamera::Status)), this, SLOT(CamError(QCamera::Status)));
}

QImage Camera::MatToQImage(cv::Mat& frame)
{
    return QImage(frame.data, frame.cols, frame.rows, frame.cols * 3, QImage::Format_RGB888);
}

void Camera::ShowImage(QLabel *place, QImage image)
{
    place->setPixmap(QPixmap::fromImage(image));
    place->setVisible(true);
    place->show();
}
