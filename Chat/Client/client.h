#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

enum Commands
{
    SIGNUP, SENDMESSAGE, NEWUSER
};

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private:
    Ui::Client *ui;
    QTcpSocket* Socket = nullptr;
    quint16 BlockSize = 0;
    QString Username = "Alexey";

private slots:
    void onSendButtonPush();
    void onAuthPushButton();
    void Authorize();
    void RecieveData();
    void onDisconnect();
};
#endif // CLIENT_H
