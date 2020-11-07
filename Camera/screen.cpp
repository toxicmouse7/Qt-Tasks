#include "screen.h"

Screen::Screen(QObject *parent) : QObject(parent)
{
    available_screens = QGuiApplication::screens();
    cur_screen.reset(available_screens[0]);
    timer.reset(new QTimer);
    timer->setInterval(27);
    connect(timer.data(), SIGNAL(timeout()), this, SLOT(GetScreenForRec()));
}

QList<QScreen *> Screen::GetAvailableScreens()
{
    return available_screens;
}

void Screen::LoadScreen(int index)
{
    cur_screen.reset(available_screens[index]);
}

void Screen::UpdateDevices()
{
    available_screens.clear();

    emit ClearComboBox();

    available_screens = QGuiApplication::screens();

    emit UpdateDevices(available_screens);
}

void Screen::ChangeScreen(int index)
{
    if (index >= 0 && index < available_screens.size())
    {
        LoadScreen(index);
    }
}

void Screen::GetScreenShot()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H-%M-%S",timeinfo);
    QString filename = static_cast<QString>(buffer);

    QPixmap screenshot = cur_screen->grabWindow(0);

    if (!QDir(QDir::currentPath() + "/screenshots/").exists())
    {
        QDir().mkdir(QDir::currentPath() + "/screenshots/");
    }

    QString filepath = QDir::currentPath() + "/screenshots/" + filename + ".png";

    screenshot.save(filepath, "PNG");
}

void Screen::GetScreenForRec()
{
    if (!QDir(QDir::currentPath() + "/screenrecord/").exists())
    {
        QDir().mkdir(QDir::currentPath() + "/screenrecord/");
    }

    QString buffer;

    if (last_used_time == 0)
    {
        last_used_time = clock();
        buffer = QString::number(last_used_time);
        starting_index = last_used_time;
    }
    else
    {
        ++last_used_time;
        buffer = QString::number(last_used_time);
    }

    QString filepath = QDir::currentPath() + "/screenrecord/" + buffer + ".png";

    if (QFile(filepath).exists())
    {
        return;
    }

    QPixmap screenshot = cur_screen->grabWindow(0);

    screenshot.save(filepath);
}

void Screen::RemoveDirectory()
{
    QDir dir(QDir::currentPath() + "/screenrecord");
    dir.removeRecursively();
}

void Screen::StartRecord(QString& filename)
{
    current_file = filename + ".avi";

    timer->start();
}

void Screen::StopRecord()
{
    timer->stop();

    QProcess proc;

    QStringList arguments =
    {
        "-f",
        "image2",
        "-r",
        "6",
        "-start_number",
        QString::number(starting_index),
        "-i",
        QDir::currentPath() + "/screenrecord/%d.png",
        "-y",
        "-an",
        "-r",
        "30",
        "-qscale",
        "0",
        QDir::currentPath() + "/Screen " + current_file
    };

    proc.start(QDir::currentPath() + "/ffmpeg/ffmpeg.exe", arguments);
    proc.waitForFinished();
    proc.close();

    QTimer::singleShot(5000, this, SLOT(RemoveDirectory()));

    last_used_time = 0;
}
