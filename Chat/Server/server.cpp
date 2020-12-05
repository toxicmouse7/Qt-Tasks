#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);

    TcpServer = new QTcpServer;

    connect(TcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    connect(ui->usrsOnlineWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(KickUser(QListWidgetItem*)));
    connect(ui->usrsOnlineWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), ui->usrsOnlineWidget, SLOT(scrollToBottom()));
    connect(ui->logsWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), ui->logsWidget, SLOT(scrollToBottom()));
    connect(ui->chatWidget->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), ui->chatWidget, SLOT(scrollToBottom()));
    connect(ui->makeAdminButton, SIGNAL(released()), this, SLOT(MakeAdmin()));

    TcpServer->listen(QHostAddress("127.0.0.1"), 4000);

    if (!TcpServer->isListening())
 {
        QMessageBox msg;
        msg.setText(TcpServer->errorString());
        msg.exec();
        parent->close();
    }
}

Server::~Server()
{
    delete ui;
}

void Server::onNewConnection()
{
    User client;

    while ((client.Socket = TcpServer->nextPendingConnection()))
    {
        if (!Clients.contains(client))
        {
            Clients.push_back(client);

            connect(client.Socket, SIGNAL(readyRead()), this, SLOT(ReadMessage()));
            connect(client.Socket, SIGNAL(disconnected()), this, SLOT(onUserDisconnect()));

            ui->logsWidget->addItem("Connection recieved!");
        }
    }
}

void Server::ReadMessage()
{
    auto Socket = qobject_cast<QTcpSocket*>(sender());
    QDataStream InputData(Socket);
    quint8 command;

    if (BlockSize == 0)
    {
        if (Socket->bytesAvailable() < (int)sizeof(quint16))
        {
            return;
        }

        InputData >> BlockSize;
    }

    if (Socket->bytesAvailable() < BlockSize)
    {
        return;
    }
    else
    {
        InputData >> command;
        BlockSize = 0;
    }

    switch (static_cast<Commands>(command))
    {
        case SENDMESSAGE:
        {
            User* client = nullptr;
            for (auto& client_ : Clients)
            {
                if (client_ == Socket)
                {
                    client = &client_;
                    break;
                }
            }

            if (client->Username == "unauthorized")
            {
                ui->logsWidget->addItem("Unauthorized user tried to send message.");
                break;
            }

            QString message;
            InputData >> message;
            if (message[0] == '/')
            {
                if (ParseCommand(message, client))
                    break;
            }
            UpdateChat(client->Username + " : " + message);
            break;
        }
        case SIGNUP:
        {
            User* client = nullptr;
            for (auto& client_ : Clients)
            {
                if (client_ == Socket)
                {
                    client = &client_;
                    break;
                }
            }

            QString Username;
            InputData >> Username;
            if (client->Username == "")
            {
                client->Username = "unauthorized";
            }
            else if(Username.contains(' '))
            {
                client->Socket->disconnectFromHost();
                return;
            }
            else
            {
                for (auto& client_ : Clients)
                {
                    if (client_.Username == Username)
                    {
                        client->Socket->disconnectFromHost();
                        return;
                    }
                }

                client->Username = Username;
            }

            SendOnlineUsers();
            ui->usrsOnlineWidget->addItem(client->Username);
            ui->logsWidget->addItem("User " + client->Username + " authorized! Online: " + QString::number(Clients.size()));
            break;
        }
        case NEWUSER:
        {
            break;
        }
    }
}

void Server::onUserDisconnect()
{
    auto Socket = qobject_cast<QTcpSocket*>(sender());
    QString Username;

    for (auto it = Clients.begin(); it != Clients.end(); ++it)
    {
        if (it->Socket == Socket)
        {
            Username = it->Username;
            for (int i = 0; ui->usrsOnlineWidget->item(i) != nullptr; ++i)
            {
                if (ui->usrsOnlineWidget->item(i)->text() == Username)
                {
                    delete ui->usrsOnlineWidget->item(i);
                    break;
                }
            }
            Clients.erase(it);
            break;
        }
    }

    SendOnlineUsers();
    ui->logsWidget->addItem(Username + " disconnected. Online: " + QString::number(Clients.size()));
}

void Server::KickUser(QListWidgetItem *item)
{
    auto Username = item->text();

    for (auto& client_ : Clients)
    {
        if (client_.Username == Username)
        {
            client_.Socket->disconnectFromHost();
        }
    }
}

void Server::MakeAdmin()
{
    auto client = ui->usrsOnlineWidget->selectedItems();
    qDebug() << client[0]->text();

    for (auto& client_ : Clients)
    {
        if (client_.Username == client[0]->text())
        {
            client_.Privilage = Server::User::privilages::admin;
            ui->logsWidget->addItem(client_.Username + " is now admin!");
            return;
        }
    }
}

void Server::UpdateChat(const QString& message)
{
    while (ui->chatWidget->count() >= 10)
    {
        delete ui->chatWidget->item(0);
    }

    QString OutMessage = '[' + QTime::currentTime().toString() + ']' + '\t' + message;

    ui->chatWidget->addItem(OutMessage);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << (quint16)0 << (quint8)Commands::SENDMESSAGE << OutMessage;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    for (auto& client : Clients)
        client.Socket->write(block);
}

void Server::SendOnlineUsers()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QList<QString> ClientsUsernames;

    for (auto& client : Clients)
    {
        ClientsUsernames.push_back(client.Username);
    }

    out << (quint16)0 << (quint8)Commands::NEWUSER << ClientsUsernames;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    for (auto& client : Clients)
    {
        client.Socket->write(block);
    }
}

bool Server::ParseCommand(QString &command, Server::User* client)
{
    QTextStream stream(&command);

    QString command_;
    stream >> command_;

    if (command_ == "/kick")
    {
        qDebug() << "ok";
        if (client->Privilage == Server::User::privilages::admin)
        {
            QString Username;
            stream >> Username;
            if (Username == client->Username)
            {
                return false;
            }

            for (int i = 0; ui->usrsOnlineWidget->item(i) != nullptr; ++i)
            {
                if (ui->usrsOnlineWidget->item(i)->text() == Username)
                {
                    KickUser(ui->usrsOnlineWidget->item(i));
                    return true;
                }
            }
        }
        else
        {
            return false;
        }
    }
    else if (command_ == "/w" || command_ == "/whisper")
    {
        QString Username;
        stream >> Username;
        if (Username == client->Username)
        {
            return false;
        }

        for (auto it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (it->Username == Username)
            {
                QByteArray blockFrom;
                QDataStream outFrom(&blockFrom, QIODevice::WriteOnly);
                QByteArray blockTo;
                QDataStream outTo(&blockTo, QIODevice::WriteOnly);

                QString message = stream.readAll();

                QString OutMessageTo = "(to " + it->Username + ')' + '[' + QTime::currentTime().toString() + ']'
                        + '\t' + message;
                QString OutMessageFrom = "(from " + client->Username + ')' + '[' + QTime::currentTime().toString() + ']'
                         + message;

                outFrom << (quint16)0 << (quint8)Commands::SENDMESSAGE << OutMessageFrom;
                outFrom.device()->seek(0);
                outFrom << (quint16)(blockFrom.size() - sizeof(quint16));

                outTo << (quint16)0 << (quint8)Commands::SENDMESSAGE << OutMessageTo;
                outTo.device()->seek(0);
                outTo << (quint16)(blockTo.size() - sizeof(quint16));

                client->Socket->write(blockTo);
                it->Socket->write(blockFrom);

                return true;
            }
        }
    }

    return false;
}

