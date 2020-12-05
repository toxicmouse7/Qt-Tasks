#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);

    connect(ui->sendButton, SIGNAL(released()), this, SLOT(onSendButtonPush()));
    connect(ui->authButton, SIGNAL(released()), this, SLOT(onAuthPushButton()));

    Socket = new QTcpSocket(this);

    connect(Socket, SIGNAL(connected()), this, SLOT(Authorize()));
    connect(Socket, SIGNAL(readyRead()), this, SLOT(RecieveData()));
    connect(Socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
}

Client::~Client()
{
    delete ui;
}

void Client::onAuthPushButton()
{
    Username = ui->leUsrnm->text();
    if (Username == "")
    {
        return;
    }
    else
    {
        ui->authButton->setEnabled(false);
        ui->leUsrnm->setEnabled(false);

        Socket->connectToHost("127.0.0.1", 4000);
    }
}

void Client::Authorize()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << (quint16)0 << (quint8)Commands::SIGNUP << Username;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    Socket->write(block);
}

void Client::RecieveData()
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

    switch(static_cast<Commands>(command))
    {
        case SENDMESSAGE:
        {
            while (ui->chatWidget->count() >= 10)
            {
                delete ui->chatWidget->item(0);
            }

            QString message;
            InputData >> message;
            ui->chatWidget->addItem(message);
            break;
        }
        case SIGNUP:
        {
            break;
        }
        case NEWUSER:
        {
            QList<QString> UserList;
            InputData >> UserList;

            ui->usrsOnlineWidget->clear();
            ui->usrsOnlineWidget->addItems(UserList);
            break;
        }
    }
}

void Client::onDisconnect()
{
    ui->usrsOnlineWidget->clear();
    QMessageBox msg;
    msg.setText("You was disconnected from server!");
    msg.exec();
    ui->authButton->setEnabled(true);
    ui->leUsrnm->setEnabled(true);
}

void Client::onSendButtonPush()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    QString message(ui->leMsg->text());
    ui->leMsg->clear();

    out << (quint16)0 << (quint8)Commands::SENDMESSAGE << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    Socket->write(block);
}

