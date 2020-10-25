#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi((QMainWindow*)this);
    cam.reset(new Camera(GetCameraPlacement()));
    cam->setParent(this);

    ConnectSignalBox(cam.data());
    QObject::connect(cam.data(), SIGNAL(ClearComboBox()), this, SLOT(ClearCamComboBox()));
    QObject::connect(cam.data(), SIGNAL(UpdateComboBox(QList<QCameraInfo>)), this, SLOT(UpdateCamComboBox(QList<QCameraInfo>)));
    QObject::connect(this, SIGNAL(CamIsReadyToUse()), cam.data(), SLOT(StandartLoad()));

    UpdateCamComboBox(cam->GetAvailableCameras());

    emit CamIsReadyToUse();

    audio.reset(new Audio);
    audio->setParent(this);

    QObject::connect(audio.data(), SIGNAL(ResetBar()), this, SLOT(ResetBar()));
    QObject::connect(GetVolumeBar(), SIGNAL(sliderMoved(int)), (QObject*)audio.data(), SLOT(setNewVolume(int)));
    QObject::connect(audio.data(), SIGNAL(SetVolume(int)), this, SLOT(SetVolume(int)));
    QObject::connect(this, SIGNAL(AudioIsReadyToUse()), audio.data(), SLOT(StandartLoad()));
    QObject::connect(ui->AvailableAudioDevices, SIGNAL(currentIndexChanged(int)), audio.data(), SLOT(ChangeAudio(int)));
    QObject::connect(audio.data(), SIGNAL(ClearComboBox()), this, SLOT(ClearAudioComboBox()));
    QObject::connect(audio.data(), SIGNAL(UpdateDevices(QList<QAudioDeviceInfo>)), this, SLOT(UpdateAudioComboBox(QList<QAudioDeviceInfo>)));

    UpdateAudioComboBox(audio->GetAvailableDevices());

    ConnectUpdateButton();

    emit AudioIsReadyToUse();

    ConnectRecordButton();
    QObject::connect(cam.data(), SIGNAL(StopAllRecords(QString)), this, SLOT(StopRecord(QString)));
}

void MainWindow::AddCamToComboBox(const QCameraInfo& caminfo)
{
    ui->AvailableCameras->addItem(caminfo.description());
}

void MainWindow::AddAudioInputToComboBox(const QAudioDeviceInfo& audInfo)
{
    ui->AvailableAudioDevices->addItem(audInfo.deviceName());
}

QSlider* MainWindow::GetVolumeBar()
{
    return ui->VolumeChange;
}

int MainWindow::GetComboIndex()
{
    return ui->AvailableCameras->currentIndex();
}

void MainWindow::ConnectSignalBox(Camera* cam)
{
    QObject::connect(ui->AvailableCameras, SIGNAL(currentIndexChanged(int)), (const QObject*)cam, SLOT(ChangeCamera(int)));
}

void MainWindow::ConnectUpdateButton()
{
    QObject::connect(ui->updateButton, SIGNAL(released()), cam.data(), SLOT(UpdateDevices()));
    QObject::connect(ui->updateButton, SIGNAL(released()), audio.data(), SLOT(UpdateDevices()));
}

void MainWindow::ConnectRecordButton()
{
    QObject::connect(ui->recordButton, SIGNAL(released()), this, SLOT(StartRecord()));
}

void MainWindow::ClearCamComboBox()
{
    ui->AvailableCameras->clear();
}

void MainWindow::ClearAudioComboBox()
{
    ui->AvailableAudioDevices->clear();
}

void MainWindow::UpdateCamComboBox(QList<QCameraInfo> availableCameras)
{
    for (auto& _camera : availableCameras)
    {
        AddCamToComboBox(_camera);
    }
}

void MainWindow::SetVolume(int volume)
{
    GetVolumeBar()->setValue(volume);
}

void MainWindow::UpdateAudioComboBox(QList<QAudioDeviceInfo> availableAudioDevices)
{
    for (auto& _audioDevice : availableAudioDevices)
    {
        AddAudioInputToComboBox(_audioDevice);
    }
}

void MainWindow::ResetBar()
{
    ui->VolumeChange->setValue(100);
}

void MainWindow::StartRecord()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H-%M-%S",timeinfo);
    QString filename = static_cast<QString>(buffer);

    ui->recordButton->setEnabled(false);
    audio->StartRecord(filename);
    cam->StartRecord(filename);
}

void MainWindow::StopRecord(QString _filename)
{
    audio->StopRecord();
    ui->recordButton->setEnabled(true);
    std::string VideoFile = "output " + _filename.toStdString() + ".avi";
    std::string AudioFile = _filename.toStdString() + ".wav";

    qDebug() << (QDir::currentPath().toStdString() + "/ffmpeg/ffmpeg.exe" + " -i " + '"' + QDir::currentPath().toStdString() + '/' + VideoFile + '"' + " -i " + '"' + QDir::currentPath().toStdString() + '/' + AudioFile + '"' + ' ' + '"' + QDir::currentPath().toStdString() + '/' + "final " + _filename.toStdString() + ".avi\"").c_str();
    QProcess* console = new QProcess;
    console->start((QDir::currentPath().toStdString() + "/ffmpeg/ffmpeg.exe" + " -i " + '"' + QDir::currentPath().toStdString() + '/' + VideoFile + '"' + " -i " + '"' + QDir::currentPath().toStdString() + '/' + AudioFile + '"' + ' ' + '"' + QDir::currentPath().toStdString() + '/' + "final " + _filename.toStdString() + ".avi\"").c_str());
    //console->waitForReadyRead();
    //system((QDir::currentPath().toStdString() + "/ffmpeg/ffmpeg.exe" + " -i " + '"' + QDir::currentPath().toStdString() + '/' + VideoFile + '"' + " -i " + '"' + QDir::currentPath().toStdString() + '/' + AudioFile + '"' + ' ' + '"' + QDir::currentPath().toStdString() + '/' + "final " + _filename.toStdString() + ".avi\"").c_str());
}

QWidget *MainWindow::GetCamsBox()
{
    return ui->AvailableCameras;
}

QSize MainWindow::GetCamWinMaxSize()
{
    QSize coords;

    coords.setWidth(ui->CameraWidget->width());
    coords.setHeight(ui->CameraWidget->height());

    return coords;
}

QWidget* MainWindow::GetCameraPlacement()
{
    return ui->CameraWidget;
}

MainWindow::~MainWindow()
{
    delete ui;
}

