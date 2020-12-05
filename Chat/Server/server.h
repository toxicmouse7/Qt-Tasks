#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTextStream>

#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

enum Commands
{
    SIGNUP, SENDMESSAGE, NEWUSER
};

class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

private:
    class User;
    Ui::Server *ui;
    QTcpServer* TcpServer = nullptr;
    QList<User> Clients;
    quint16 BlockSize = 0;

    class User{
    public:
        enum privilages
        {
          user, admin
        };

        QString Username = "unauthorized";
        privilages Privilage = user;
        QTcpSocket* Socket = nullptr;

        bool operator==(const QString& s)
        {
            return Username == s;
        }

        bool operator==(const QTcpSocket* obj)
        {
            return Socket == obj;
        }

        bool operator==(const User& obj)
        {
            return this->Username == obj.Username && this->Socket == obj.Socket;
        }
    };

    void UpdateChat(const QString& message);
    void SendOnlineUsers();
    bool ParseCommand(QString& command, User* client);

private slots:
    void onNewConnection();
    void ReadMessage();
    void onUserDisconnect();
    void KickUser(QListWidgetItem *item);
    void MakeAdmin();

};
#endif // SERVER_H
