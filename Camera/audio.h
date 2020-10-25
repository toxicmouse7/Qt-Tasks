#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QIODevice>
#include <QAudioInput>
#include <QAudioRecorder>
#include <QAudioDeviceInfo>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QUrl>
#include <QDir>

class Audio : public QObject
{
    Q_OBJECT
public:
    Audio();
    const QList<QAudioDeviceInfo>& GetAvailableDevices();

private:
    QList<QAudioDeviceInfo> availableDevices;
    QAudioDeviceInfo device_info;
    QScopedPointer<QAudioInput> curAudioDevice;
    QScopedPointer<QFile> output_file;
    QScopedPointer<QAudioRecorder> recorder;

    void LoadAudio();
    void LoadAudio(int index);

signals:
    void SetVolume(int);
    void ResetBar();
    void ClearComboBox();
    void UpdateDevices(QList<QAudioDeviceInfo>);

public slots:
    void setNewVolume(int volume);
    void StandartLoad();
    void ChangeAudio(int index);
    void StartRecord(QString& filename);
    void StopRecord();
    void UpdateDevices();

};

#endif // AUDIO_H
