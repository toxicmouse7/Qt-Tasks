#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QMessageBox>
#include <QtWidgets>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraInfo>
#include <QtMultimedia/QCameraImageCapture>
#include <QtMultimedia/QCameraFocus>
#include <QtMultimediaWidgets/QCameraViewfinder>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <QTimer>

class Camera : public QObject
{
    Q_OBJECT

private:
    int curCameraIndex = 0;
    QWidget* camera_placement;
    QList<QCameraInfo> availableCameras;
    QScopedPointer<QCamera> curCamera;
    QList<QSize> resolutions;
    QCameraFocus* cameraFocus = nullptr;
    QScopedPointer<QCameraViewfinder> curCameraViewFinder;
    QScopedPointer<QCameraViewfinderSettings> curCameraViewFinderSettings;
    QScopedPointer<QCameraImageCapture> imageCapture;
    cv::VideoCapture recorder;
    QScopedPointer<cv::VideoWriter> writer;
    QScopedPointer<QTimer> timer;

    void LoadCamera();
    void LoadCamera(int index);
    void ConnectCameraStatus();
    QImage MatToQImage(cv::Mat& frame);
    void ShowImage(QLabel* place, QImage image);

public:
    Camera(QWidget* CameraPlacement);
    QList<QCameraInfo> GetAvailableCameras();

signals:
    void cameraUnavailable(bool IsAvailable);
    void CameraReadyToUse();
    void ClearComboBox();
    void UpdateComboBox(QList<QCameraInfo>);
    void StopAllRecords(QString);


public slots:
    void UpdateDevices();
    void ChangeCamera(int index);
    void CamError(QCamera::Status status);
    void StartRecord(QString& filename);
    void StopRecord();
    void StandartLoad();
    void cvRecord();

};

#endif // CAMERA_H
