#include "gisclient.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QTextCodec>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QThread>

GisClient::GisClient(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    blockSize = 0;

    QSettings settings;
    QString ip = settings.value("GisServer/serverIP").toString();
    int port = settings.value("GisServer/serverPort").toInt();

    tcpSocket->connectToHost(ip, port);

    connect(tcpSocket, SIGNAL(connected()), SIGNAL(connected()));
    connect(tcpSocket, SIGNAL(readyRead()), SLOT(printReadData()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(printError(QAbstractSocket::SocketError)));
}

GisClient::~GisClient()
{

}

void GisClient::printError(QAbstractSocket::SocketError error)
{
    const QString strError = tr("Error: ") + (error == QAbstractSocket::HostNotFoundError ?
                                        tr("The host was not found.") :
                                        error == QAbstractSocket::RemoteHostClosedError ?
                                            tr("The remote host is closed.") :
                                            error == QAbstractSocket::ConnectionRefusedError ?
                                                tr("The connection was refused.") :
                                                QString(tcpSocket->errorString())
                                                );
    qDebug() << strError;
    emit this->error(strError);
}

void GisClient::printReadData()
{
    QByteArray data;

    while (!tcpSocket->atEnd()) {
        data = tcpSocket->readAll();
    }

    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    QString str = codec->toUnicode(data);

    if (str.contains(".ACT GETMAP__")) {
        QRegExp regExp("\\[DATA\\]\\s\\.MAP\\s(.*\\.(MAP|SIT|map|sit))");
        QStringList mapsList = QStringList();
        int pos = 0;
        while ((pos = regExp.indexIn(str.simplified(), pos)) != -1) {
            mapsList << regExp.cap(1);
            pos += regExp.matchedLength();
        }
        mapReady(mapsList);
    }
}

void GisClient::loadMap()
{
    QSettings settings;
    QString pathToMap = settings.value("GisServer/map", "").toString();

    QStringList command(QStringList()
                        << "[CONTROL]"
                        << ".ACT LOAD____"
                        << ".MAP " + pathToMap
                        << ".SIT " + QDir::toNativeSeparators(QFileInfo(pathToMap).absolutePath()) + "\\test.sit"
                        << ".ASK 745"
                        << ".END");
    sendCommand(command.join("\n"));
}

void GisClient::sendCommand(const QString &command)
{
    QByteArray block;

    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    block.append(codec->fromUnicode(command));

    tcpSocket->write(block);
    tcpSocket->write("\n");
}

void GisClient::createObject(int VP, int KP, int NP, const QStringList &coordinatsStream)
{
    QSettings settings;
    QString pathToMap = settings.value("GisServer/map", "").toString();

    QStringList command(QStringList()
                        << "[CONTROL]"
                        << ".ACT DATA____"
                        << ".MAP " + pathToMap
                        << ".SIT " + QDir::toNativeSeparators(QFileInfo(pathToMap).absolutePath()) + "\\test.sit"
                        << ".ASK 345"
                        << ".REQ 1"
                        << "[DATA]"
                        << ".SIT"
                        << ".OBJ " + QString("STREAM%1%2%3").arg(VP).arg(KP).arg(NP)
                        << ".KEY L10000000" + QString::number(VP)
                        << ".SPL POINTS"
                        << ".MET 1"
                        << QString("%1").arg(coordinatsStream.size())
                        << coordinatsStream.join("\n")
                        << ".END");

    sendCommand(command.join("\n"));
}

void GisClient::removeData()
{
//    QStringList command(QStringList()
//                        << "[CONTROL]"
//                        << ".ACT DELETE____"
//                        << ".SIT  " + QDir(fileMap).absolutePath() + "/test.sit"
//                        << ".ASK 255"
//                        << ".END");
//    sendCommand(command.join("\n"));
}

void GisClient::getMap()
{
    QStringList command(QStringList() << "[CONTROL]" << ".ACT GETMAP__" << ".ASK 1261" << ".END");
    sendCommand(command.join("\n"));
}
