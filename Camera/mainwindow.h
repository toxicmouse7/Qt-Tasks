#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QCameraInfo>
#include <QAudioDeviceInfo>
#include <QSlider>
#include <QDir>

#include "audio.h"
#include "camera.h"
//class Camera;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QScopedPointer<Camera> cam;
    QScopedPointer<Audio> audio;

    QWidget* GetCameraPlacement();
    QSize GetCamWinMaxSize();
    void AddCamToComboBox(const QCameraInfo& caminfo);
    void AddAudioInputToComboBox(const QAudioDeviceInfo& audInfo);
    QSlider* GetVolumeBar();
    QWidget* GetCamsBox();
    void ConnectSignalBox(Camera* cam);
    void ConnectUpdateButton();
    void ConnectRecordButton();
    int GetComboIndex();

public slots:
    void ClearCamComboBox();
    void ClearAudioComboBox();
    void UpdateCamComboBox(QList<QCameraInfo> availableCameras);
    void SetVolume(int volume);
    void UpdateAudioComboBox(QList<QAudioDeviceInfo> availableAudioDevices);
    void ResetBar();
    void StartRecord();
    void StopRecord(QString _filename);

signals:
    void CamIsReadyToUse();
    void AudioIsReadyToUse();


};
#endif // MAINWINDOW_H
