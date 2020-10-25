#include "audio.h"

Audio::Audio()
{
    availableDevices = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);
    recorder.reset(new QAudioRecorder);
}

void Audio::LoadAudio()
{
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    device_info = availableDevices[0];

    if (availableDevices[0].isFormatSupported(format))
    {
        format = availableDevices[0].nearestFormat(format);
    }

    curAudioDevice.reset(new QAudioInput(availableDevices[0], format));
    qreal initialVolume = QAudio::convertVolume(curAudioDevice->volume(),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);

    emit SetVolume(qRound(initialVolume * 100));

    curAudioDevice->start();
}

void Audio::LoadAudio(int index)
{
    if (!curAudioDevice.isNull())
    {
        curAudioDevice->setVolume(1);
    }

    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    device_info = availableDevices[index];

    if (availableDevices[index].isFormatSupported(format))
    {
        format = availableDevices[index].nearestFormat(format);
    }

    curAudioDevice.reset(new QAudioInput(availableDevices[index], format));
    qreal initialVolume = QAudio::convertVolume(curAudioDevice->volume(),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);

    emit SetVolume(qRound(initialVolume * 100));

    curAudioDevice->start();
}

const QList<QAudioDeviceInfo>& Audio::GetAvailableDevices()
{
    return availableDevices;
}

void Audio::setNewVolume(int volume)
{
    qreal initialVolume = QAudio::convertVolume(volume / (qreal)100,
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);
    curAudioDevice->setVolume(initialVolume);
}

void Audio::StandartLoad()
{
    if (availableDevices.size() > 0)
    {
        LoadAudio();
        emit ResetBar();
    }
    else
    {
        QMessageBox msg;
        msg.setText("No audio devices detected");
        msg.exec();
    }
}

void Audio::ChangeAudio(int index)
{
    if (index < availableDevices.size())
    {
        emit ResetBar();
        LoadAudio(index);
    }
}

void Audio::StartRecord(QString& filename)
{
    recorder->setAudioInput(device_info.deviceName());

    QAudioEncoderSettings settings;
    settings.setCodec(availableDevices[0].supportedCodecs()[0]);
    settings.setSampleRate(16000);
    settings.setBitRate(32000);
    settings.setChannelCount(1);
    settings.setQuality(QMultimedia::NormalQuality);
    settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    recorder->setOutputLocation(QUrl::fromLocalFile(QDir::currentPath() + '/' + filename));

    QString container = "audio/x-wav";

    recorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
    recorder->record();
}

void Audio::StopRecord()
{
    recorder->stop();
}

void Audio::UpdateDevices()
{
    availableDevices.clear();

    emit ClearComboBox();

    availableDevices = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);

    emit UpdateDevices(availableDevices);
}
