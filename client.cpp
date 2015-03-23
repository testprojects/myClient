#include "client.h"
#include <QtNetwork>
#include "packet.h"
#include "../plan/station.h"
#include "assert.h"
#include "../plan/pauser.h"

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
    out << (quint32)0;
    out << message;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    m_tcpSocket->write(block);
    m_tcpSocket->flush();
    qDebug() << "Sended message: " << message;
    qDebug() << "Block size    : " << (quint32)(block.size() - sizeof(quint32));
}

void Client::readPacket()
{
    quint32 blockSize;
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    while(m_tcpSocket->bytesAvailable()) {
        while(m_tcpSocket->bytesAvailable() < (int)sizeof(quint32)) {}
        in >> blockSize;
        while(m_tcpSocket->bytesAvailable() < blockSize) {
            QCoreApplication::processEvents();
            //readyRead() is not emitted recursively; if you reenter the event loop or call waitForReadyRead()
            //inside a slot connected to the readyRead() signal, the signal will not be reemitted (although waitForReadyRead()
            //may still return true).
            //Отдельному обработчику событий нужен доступ к сокету, чтобы в
            Pauser pauser(m_tcpSocket);

            QTimer timer;
            timer.setInterval(100);
            QObject::connect(&timer, SIGNAL(timeout()), &pauser, SLOT(checkIfDataRecieved()));
            timer.start();
            pauser.exec();
        }
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
        case TYPE_XML_F2:
        {
            emit signalF2Ready(block);
            break;
        }
        default:
        {
            assert(0);
        }
        }
    }
}

void Client::dispatchMessage(QString message)
{
    qDebug() << "Despatching message: " << message;
    if(message.startsWith("PLAN_STARTED")) {
        emit signalPlanStarted();
    }
    else if(message.startsWith("STREAM_PLANNED")) {
        message.remove(0, QString::fromUtf8("STREAM_PLANNED(").length());
        message.chop(1);
        QStringList list = message.split('/');
        int count = list[0].toInt();
        int amount = list[1].toInt();
        emit signalPlanned(count, amount);
    }
    else if(message.startsWith("FAILED_STREAMS")) {
        message.remove(0, QString::fromUtf8("FAILED_STREAMS(").length());
        message.chop(1);
        int count = message.toInt();
        emit signalStreamsFailed(count);
    }
    else if(message.startsWith("PLAN_FINISHED")) {
        emit signalPlanFinished();
    }
    else if(message.startsWith("OFFSET_STREAM")) {
        message.remove(0, QString::fromUtf8("OFFSET_STREAM(").length());
        message.chop(1);
        QStringList list = message.split(',');
        int VP = list[0].toInt();
        int KP = list[1].toInt();
        int NP = list[2].toInt();
        int hours = list[3].toInt();
        emit signalOffsetStream(VP, KP, NP, hours);
    }
    else if(message.startsWith("REQUESTS_ADDED")) {
        qDebug() << "Requests added to server";
    }
}
