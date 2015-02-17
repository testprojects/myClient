#include "client.h"
#include <QtNetwork>
#include <QDebug>
#include <iostream>

const int CONNECTION_INTERVAL = 1000;

Client::Client()
{
    m_tcpSocket = new QTcpSocket;
    //убираем задержку перед отсоединением сокета, если есть проблемы с серваком
    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 0);
}

void Client::connectToServer(QString serverIP, quint16 serverPort)
{
    //что будет, если пытаться установить соединение, которое уже установлено?
    m_serverIP = serverIP;
    m_serverPort = serverPort;
    QTimer *connectTimer = new QTimer(this);
    connectTimer->setInterval(CONNECTION_INTERVAL);

    connect(connectTimer, SIGNAL(timeout()), this, SLOT(tryToConnect()));
    connect(m_tcpSocket, SIGNAL(connected()), connectTimer, SLOT(stop()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    connectTimer->start();
}

void Client::disconnectFromServer()
{
    m_tcpSocket->disconnectFromHost();
}

void Client::tryToConnect()
{
    m_tcpSocket->connectToHost(QHostAddress(m_serverIP), m_serverPort);
}

bool Client::isConnected() const
{
    if(m_tcpSocket->state() == QAbstractSocket::ConnectedState)
        return true;
    return false;
}

Client::~Client()
{
    m_tcpSocket->disconnectFromHost();
    connect(m_tcpSocket, SIGNAL(disconnected()), SLOT(deleteLater()));
}

void Client::sendMessage(QString message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    m_tcpSocket->write(block);
    m_tcpSocket->flush();
}

//void Client::readMessage()
//{
//    quint16 blockSize;
//    QDataStream in(m_tcpSocketIn);
//    in.setVersion(QDataStream::Qt_4_0);

//    if (m_tcpSocketIn->bytesAvailable() < (int)sizeof(quint16)) {
//        return;
//    }
//    in >> blockSize;

//    if (m_tcpSocketIn->bytesAvailable() < blockSize) {
//        return;
//    }

//    in >> m_serverMessage;
//    m_serverMessage.remove('\"');
//    dispatchServerMessage();
//}
