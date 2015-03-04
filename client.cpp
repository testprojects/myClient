#include "client.h"
#include <QtNetwork>
#include "packet.h"
#include "station.h"
#include "assert.h"

const int CONNECTION_INTERVAL = 1000;

Client::Client(QString serverIP, quint16 serverPort, QObject *parent):
    QObject(parent), m_serverIP(serverIP), m_serverPort(serverPort)
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readPacket()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(this, SIGNAL(stringRecieved(QString)), SLOT(dispatchMessage(QString)));
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

void Client::sendMessage(const QString &message)
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

void Client::readPacket()
{
    quint16 blockSize;
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    while(m_tcpSocket->bytesAvailable()) {
        while(m_tcpSocket->bytesAvailable() < (int)sizeof(quint16)) {}
        in >> blockSize;
        while(m_tcpSocket->bytesAvailable() < blockSize) {}
        Type type;
        quint8 t;
        in >> t;
        type = (Type)t;
        QByteArray block;
        block = m_tcpSocket->read(blockSize - sizeof(quint8));

        qDebug() << "bytes available: " << m_tcpSocket->bytesAvailable();
        qDebug() << "block size     : " << blockSize;
        qDebug() << "block          : " << block;

        switch(type) {
        case TYPE_QSTRING:
        {
            Packet pack(block, TYPE_QSTRING);
            QString str = pack.decodeToQString();
            emit stringRecieved(str);
            break;
        }
        case TYPE_STATION:
        {
            Packet pack(block, TYPE_STATION);
            Station st = pack.decodeToStation();
            QString strStation = st.name + QString(" (%1)").arg(st.number);
            emit stringRecieved(strStation);
            break;
        }
        default:
        {
            assert(0);
        }
        }
    }
}

void Client::dispatchMessage(const QString &message)
{
    qDebug() << "Despatching message: " << message;
    if(message.startsWith("PLAN_STARTED")) {
        emit signalPlanStarted();
    }
    else if(message.startsWith("STREAM_PLANNED")) {
        int count = message.mid(message.indexOf('(') + 1, message.indexOf('/') - message.indexOf('(') - 1).toInt();
        int amount = message.mid(message.indexOf('/') + 1, message.indexOf(')') - message.indexOf('/') - 1).toInt();
        emit signalPlanned(count, amount);
    }
    else if(message.startsWith("FAILED_STREAMS")) {
        int count = message.mid(message.indexOf('(') + 1, message.indexOf(')') - message.indexOf('(')).toInt();
        emit signalStreamsFailed(count);
    }
    else if(message.startsWith("PLAN_FINISHED")) {
        emit signalPlanFinished();
    }
}
