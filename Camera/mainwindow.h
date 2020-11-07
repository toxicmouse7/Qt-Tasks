#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QCameraInfo>
#include <QAudioDeviceInfo>
#include <QSlider>
#include <QDir>
#include <QTimer>

#include <audio.h>
#include <camera.h>
#include <screen.h>
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
    QScopedPointer<Screen> screen;
    QString output_filename;
    bool is_recording_camera;

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

private slots:
    void ClearCamComboBox();
    void ClearAudioComboBox();
    void ClearScreenComboBox();
    void UpdateCamComboBox(QList<QCameraInfo> availableCameras);
    void SetVolume(int volume);
    void UpdateAudioComboBox(QList<QAudioDeviceInfo> availableAudioDevices);
    void ResetBar();
    void StartRecordCameraAndAudio();
    void StopRecordCameraAndAudio();
    void StartRecordScreenAndAudio();
    void StopRecordScreenAndAudio();
    void StopRecord();
    void UpdateScreenComboBox(QList<QScreen *> availableScreens);
    void RemoveFile(QString& filename);

signals:
    void CamIsReadyToUse();
    void AudioIsReadyToUse();


};
#endif // MAINWINDOW_H
