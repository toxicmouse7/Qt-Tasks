#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QWidget>
#include <QDir>
#include <QProcess>
#include <QTimer>

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QObject *parent = nullptr);
    QList<QScreen *> GetAvailableScreens();

private:
    QList<QScreen*> available_screens;
    QScreen* cur_screen = QGuiApplication::primaryScreen();
    QString current_file;
    int starting_index = 0;
    clock_t last_used_time = 0;
    QScopedPointer<QTimer> timer;

    void LoadScreen(int index);

signals:
    void UpdateDevices(QList<QScreen*>);
    void ClearComboBox();

private slots:
    void UpdateDevices();
    void ChangeScreen(int index);
    void GetScreenShot();
    void GetScreenForRec();
    void RemoveDirectory();

public slots:
    void StopRecord();
    void StartRecord(QString& filename);

};

#endif // SCREEN_H
