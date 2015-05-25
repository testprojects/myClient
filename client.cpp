#include "client.h"
#include <QtNetwork>
#include "packet.h"
#include "../plan/station.h"
#include "assert.h"
#include "../plan/pauser.h"
#include <QMessageBox>

//const int CONNECTION_INTERVAL = 1000;
const int MAX_CONNECTION_TIME = 5000;

Client::Client(QString serverIP, quint16 serverPort, QObject *parent):
    QObject(parent), m_serverIP(serverIP), m_serverPort(serverPort), m_blockSize(0)
{
    obstacleTime = new QTime();
    connectTimer = new QTimer(this);
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readPacket()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(this, SIGNAL(stringRecieved(QString)), SLOT(dispatchMessage(QString)));
}

void Client::connectToServer()
{
    connectTimer->setInterval(MAX_CONNECTION_TIME);
    connectTimer->setSingleShot(true);

    connect(connectTimer, SIGNAL(timeout()), this, SLOT(disconnectFromServer()));
    connect(this, SIGNAL(connected()), connectTimer, SLOT(stop()));

    connectTimer->start();
    m_tcpSocket->connectToHost(QHostAddress(m_serverIP), m_serverPort);
}

void Client::disconnectFromServer()
{
    if(!isConnected()) {
        m_tcpSocket->abort();
        emit disconnected();
    }
    else {
        m_tcpSocket->disconnectFromHost();
    }
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
    qDebug() << "Sended message: " << block << endl;
    qDebug() << "Block size    : " << (quint32)(block.size() - sizeof(quint32));
}

void Client::readPacket()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    while(true) {
        if(!m_blockSize) {
            if(m_tcpSocket->bytesAvailable() < (int)sizeof(quint32)) {
                return;
            }
            in >> m_blockSize;
        }

        if(m_tcpSocket->bytesAvailable() < m_blockSize) {
            return;
        }
        qDebug() << QString("%1 recieved").arg(m_blockSize);
        Type type;
        quint8 t;
        in >> t;
        type = (Type)t;
        QByteArray block;
        block = m_tcpSocket->read(m_blockSize - sizeof(quint8));

        qDebug() << "bytes available: " << m_tcpSocket->bytesAvailable();
        qDebug() << "block size     : " << m_blockSize;
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
            case TYPE_XML_STREAMS:
            {
                emit signalStreamsReady(block);
                break;
            }
            default:
            {
                assert(0);
            }
        }

        m_blockSize = 0;
    }
}

void Client::dispatchMessage(QString message)
{
//    qDebug() << "Despatching message: " << message;
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
        QString strPassedStations = list[0];
        QString strDepartureTime = list[1];
        int NP = list[2].toInt();
        int hours = list[3].toInt();
        emit signalOffsetStream(strPassedStations, strDepartureTime, NP, hours);
    }
    else if(message.startsWith("STREAM_PLAN_FAILED")){
        //приостанавливаем планирование
        emit signalPausePlanning();

        message.remove(0, QString::fromUtf8("STREAM_PLAN_FAILED(").length());
        message.chop(1);
        message.append(QString::fromUtf8(". Продолжить планирование?"));
        int answer;
        answer = QMessageBox::warning(NULL, QString::fromUtf8("Поток не спланирован"), message, QMessageBox::Yes, QMessageBox::No);
        if(answer == QMessageBox::Yes) {
            //продолжить планирование
            emit signalContinuePlanning();
        }
        else {
            emit signalAbortPlanning();
            //прекратить планирование
        }
    }
    else if(message.startsWith("REQUESTS_ADDED")) {
        qDebug() << "Requests added to server";
    }
}

void Client::setServerIP(QString serverIP) {
    m_serverIP = serverIP;
}

QString Client::serverIP() {
    return m_serverIP;
}

void Client::setServerPort(quint16 port) {
    m_serverPort = port;
}

quint16 Client::serverPort() {
    return m_serverPort;
}
