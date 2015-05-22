#ifndef GISCLIENT_H
#define GISCLIENT_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;
class GisClient : public QObject
{
        Q_OBJECT
    public:
        explicit GisClient(QObject *parent = 0);
        ~GisClient();

        void loadMap();
        void createObject();
        void removeData();

    private:
        void sendCommand(const QString&);

        QTcpSocket *tcpSocket;
        quint16 blockSize;
        QString fileMap;

    private slots:
        void printError(QAbstractSocket::SocketError);
        void printReadData();

    signals:
        void connected();
        void error(const QString&);
};

#endif // GISCLIENT_H
