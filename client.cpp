#include "client.h"
#include <QtNetwork>

const int CONNECTION_INTERVAL = 1000;

Client::Client(QString serverIP, quint16 serverPort, QObject *parent):
    QObject(parent), m_serverIP(serverIP), m_serverPort(serverPort)
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    //убираем задержку перед отсоединением сокета, если есть проблемы с серваком
//    m_tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 0);
}

void Client::connectToServer()
{
    //что будет, если пытаться установить соединение, которое уже установлено?
    QTimer *connectTimer = new QTimer(this);
    connectTimer->setInterval(CONNECTION_INTERVAL);
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(tryToConnect()));
    connect(m_tcpSocket, SIGNAL(connected()), connectTimer, SLOT(stop()));
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

void Client::readMessage()
{
    quint16 blockSize;
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
        return;
    }
    in >> blockSize;

    if (m_tcpSocket->bytesAvailable() < blockSize) {
        return;
    }

    in >> m_serverMessage;
//    qDebug() << "Message readed: " << m_serverMessage;
    emit messageRecieved(m_serverMessage);
}
