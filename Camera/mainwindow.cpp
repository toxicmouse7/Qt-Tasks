#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi((QMainWindow*)this);

    QPixmap bkgnd("backgr.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

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

    emit AudioIsReadyToUse();

    screen.reset(new Screen);
    screen->setParent(this);

    connect(screen.data(), SIGNAL(ClearComboBox()), this, SLOT(ClearScreenComboBox()));
    connect(screen.data(), SIGNAL(UpdateDevices(QList<QScreen*>)), this, SLOT(UpdateScreenComboBox(QList<QScreen*>)));
    connect(ui->AvailableScreens, SIGNAL(currentIndexChanged(int)), screen.data(), SLOT(ChangeScreen(int)));
    connect(ui->screenshotButton, SIGNAL(released()), screen.data(), SLOT(GetScreenShot()));

    UpdateScreenComboBox(screen->GetAvailableScreens());

    ConnectUpdateButton();

    ConnectRecordButton();
    connect(ui->stopRecord, SIGNAL(released()), this, SLOT(StopRecord()));
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
    connect(ui->updateButton, SIGNAL(released()), screen.data(), SLOT(UpdateDevices()));
}

void MainWindow::ConnectRecordButton()
{
    QObject::connect(ui->recordButton, SIGNAL(released()), this, SLOT(StartRecordCameraAndAudio()));
    connect(ui->recordScreenAudButton, SIGNAL(released()), this, SLOT(StartRecordScreenAndAudio()));
}

void MainWindow::ClearCamComboBox()
{
    ui->AvailableCameras->clear();
}

void MainWindow::ClearAudioComboBox()
{
    ui->AvailableAudioDevices->clear();
}

void MainWindow::ClearScreenComboBox()
{
    ui->AvailableScreens->clear();
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

void MainWindow::StartRecordCameraAndAudio()
{
    is_recording_camera = true;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H-%M-%S",timeinfo);
    QString filename = static_cast<QString>(buffer);

    output_filename = filename;

    ui->recordButton->setEnabled(false);
    ui->recordScreenAudButton->setEnabled(false);
    ui->stopRecord->setEnabled(true);
    ui->updateButton->setEnabled(false);
    cam->StartRecord(filename);
    audio->StartRecord(filename);
}

void MainWindow::StopRecordCameraAndAudio()
{
    audio->StopRecord();
    cam->StopRecord();
    ui->recordButton->setEnabled(true);
    ui->recordScreenAudButton->setEnabled(true);
    ui->stopRecord->setEnabled(false);
    ui->updateButton->setEnabled(true);
    QString VideoFile = "output " + output_filename + ".avi";
    QString AudioFile = output_filename + ".wav";

    const QStringList arguments =
    {
        "-i",
        QDir::currentPath() + '/' + VideoFile,
        "-i",
        QDir::currentPath() + '/' + AudioFile,
        "-qscale",
        "0",
        QDir::currentPath() + '/' + "camera and audio " + output_filename + ".avi"
    };

    QProcess proc;
    proc.start(QDir::currentPath() + "/ffmpeg/ffmpeg.exe", arguments);
    proc.waitForFinished();
    proc.close();

    RemoveFile(VideoFile);
    RemoveFile(AudioFile);
}

void MainWindow::StartRecordScreenAndAudio()
{
    is_recording_camera = false;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H-%M-%S",timeinfo);
    QString filename = static_cast<QString>(buffer);

    output_filename = filename;

    ui->recordButton->setEnabled(false);
    ui->recordScreenAudButton->setEnabled(false);
    ui->stopRecord->setEnabled(true);
    ui->updateButton->setEnabled(false);
    audio->StartRecord(filename);
    screen->StartRecord(filename);
}

void MainWindow::StopRecordScreenAndAudio()
{
    audio->StopRecord();
    screen->StopRecord();
    ui->recordButton->setEnabled(true);
    ui->recordScreenAudButton->setEnabled(true);
    ui->stopRecord->setEnabled(false);
    ui->updateButton->setEnabled(true);
    QString ScreenFile = "Screen " + output_filename + ".avi";
    QString AudioFile = output_filename + ".wav";

    const QStringList arguments =
    {
        "-i",
        QDir::currentPath() + '/' + ScreenFile,
        "-i",
        QDir::currentPath() + '/' + AudioFile,
        "-qscale",
        "0",
        QDir::currentPath() + '/' + "screen and audio " + output_filename + ".avi"
    };

    QProcess proc;
    proc.start(QDir::currentPath() + "/ffmpeg/ffmpeg.exe", arguments);
    proc.waitForFinished();
    proc.close();

    RemoveFile(ScreenFile);
    RemoveFile(AudioFile);
}

void MainWindow::StopRecord()
{
    if (is_recording_camera)
    {
        StopRecordCameraAndAudio();
    }
    else
    {
        StopRecordScreenAndAudio();
    }
}

void MainWindow::UpdateScreenComboBox(QList<QScreen *> availableScreens)
{
    for (auto& screen : availableScreens)
    {
        ui->AvailableScreens->addItem(screen->name());
    }
}

void MainWindow::RemoveFile(QString &filename)
{
    QFile file(filename);
    file.remove();
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

