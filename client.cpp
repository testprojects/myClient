#include "client.h"
#include <QtNetwork>
#include <QDebug>
#include <iostream>

const int CONNECTION_INTERVAL = 1000;

Client::Client()
{
    m_tcpSocketOut = new QTcpSocket;
    m_tcpSocketIn =  new QTcpSocket;
    //убираем задержку перед отсоединением сокета, если есть проблемы с серваком
    m_tcpSocketOut->setSocketOption(QAbstractSocket::KeepAliveOption, 0);
    m_tcpSocketIn->setSocketOption(QAbstractSocket::KeepAliveOption, 0);
}

void Client::connectToServer(QString serverIP, quint16 serverPort)
{
    //что будет, если пытаться установить соединение, которое уже установлено?
    m_serverIP = serverIP;
    m_serverPort = serverPort;
    QTimer *connectTimer = new QTimer(this);
    connectTimer->setInterval(CONNECTION_INTERVAL);
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(tryToConnectIn()));
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(tryToConnectOut()));
    connectTimer->start();

    //для разрыва связи с таймером
    connect(m_tcpSocketOut, SIGNAL(connected()), this, SIGNAL(connectedOut()));
    connect(m_tcpSocketIn,  SIGNAL(connected()), this, SIGNAL(connectedIn()));

    //для вызова connected для UI
    connect(m_tcpSocketOut, SIGNAL(connected()), this, SLOT(emitConnect()));
    connect(m_tcpSocketIn,  SIGNAL(connected()), this, SLOT(emitConnect()));

    //для вызова disconnected для UI
    connect(m_tcpSocketOut, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(m_tcpSocketIn,  SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    connect(this, SIGNAL(connected()), connectTimer, SLOT(stop()));
}

void Client::emitConnect()
{
    //если оба сокета подключены, клиент подключен полностью
    if(isConnected()) {
        emit connected();
    }
}

void Client::emitDisconnect()
{
    emit disconnected();
}

void Client::disconnectFromServer()
{
    m_tcpSocketOut->disconnectFromHost();
    m_tcpSocketIn->disconnectFromHost();
}

void Client::tryToConnectOut()
{
    if(!isConnectedOut())
        m_tcpSocketOut->connectToHost(QHostAddress(m_serverIP), m_serverPort);
}

void Client::tryToConnectIn()
{
    if(!isConnectedIn())
        m_tcpSocketIn->connectToHost(QHostAddress(m_serverIP), m_serverPort);
}

bool Client::isConnected() const
{
    if(isConnectedIn() && isConnectedOut())
        return true;
    return false;
}

bool Client::isConnectedOut() const
{
    if(m_tcpSocketOut->state() == QTcpSocket::ConnectedState)
        return true;
    return false;
}

bool Client::isConnectedIn() const
{
    if(m_tcpSocketIn->state() == QTcpSocket::ConnectedState)
        return true;
    return false;
}

Client::~Client()
{
    m_tcpSocketOut->disconnectFromHost();
    connect(m_tcpSocketOut, SIGNAL(disconnected()), SLOT(deleteLater()));
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

    m_tcpSocketOut->write(block);
    m_tcpSocketOut->flush();
}

void Client::readMessage()
{
    quint16 blockSize;
    QDataStream in(m_tcpSocketIn);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_tcpSocketIn->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    in >> blockSize;

    if (m_tcpSocketIn->bytesAvailable() < blockSize) {
        return;
    }

    in >> m_serverMessage;
    m_serverMessage.remove('\"');
    dispatchServerMessage();
}

void Client::dispatchServerMessage()
{
    //в зависимости от того, какое сообщение принято от сервера
    //вызываем то или иное меню
    //сервер будет ждать указания, какую команду обработать
}
