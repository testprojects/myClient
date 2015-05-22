#include "gisclient.h"

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QTextCodec>
#include <QtCore/QSettings>
#include <QtCore/QDir>

GisClient::GisClient(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    blockSize = 0;

    QSettings settings;
    fileMap = settings.value("Gis/pathToMap").toString();

    tcpSocket->connectToHost(QHostAddress::LocalHost, 1024);

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
    QDataStream in(tcpSocket);
//    in.setVersion(QDataStream::Qt_5_3);

//    while (true) {
//        if (!blockSize) {
//            if (tcpSocket->bytesAvailable() < sizeof(quint16)) {
//                break;
//            }
//            in >> blockSize;
//        }
//        if (tcpSocket->bytesAvailable() < blockSize) {
//            break;
//        }

        QString str;
        in >> str;

        qDebug() << str;

//        blockSize = 0;
//    }
}

void GisClient::loadMap()
{
    QStringList command(QStringList()
                        << "[CONTROL]"
                        << ".ACT LOAD____"
                        << ".MAP " + fileMap
                        << ".SIT " + QDir::toNativeSeparators(QFileInfo(fileMap).absolutePath()) + "\\test.sit"
                        << ".ASK 745"
                        << ".END");
    sendCommand(command.join("\n"));
    qDebug() << QFileInfo(fileMap).absolutePath();
}

void GisClient::sendCommand(const QString &command)
{
    QByteArray block;

    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    block.append(codec->fromUnicode(command));

    tcpSocket->write(block);
}

void GisClient::createObject()
{
    QStringList command(QStringList()
                        << "[CONTROL]"
                        << ".ACT DATA____"
                        << ".MAP C:\\Program Files (x86)\\Panorama\\Operator11\\Data\\Cadastre\\88-2.MAP"
                        << ".SIT C:\\Program Files (x86)\\Panorama\\Operator11\\Data\\Cadastre\\test.SIT"
                        << ".ASK 345"
                        << ".REQ 1"
                        << "[DATA]"
                        << ".SIT"
                        << ".OBJ 0"
                        << ".KEY L_OBJECT_NEW"
                        << ".SPL POINTS"
                        << ".MET 1"
                        << "3"
                        << "5925.28   -4986.85"
                        << "5925.28   -4675.39"
                        << "5727.28   -4690.39"
                        << ".SYS MET42"
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
