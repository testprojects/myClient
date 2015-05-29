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

        void createObject(int, int, int, const QStringList&);
        void removeData();

    private:
        void sendCommand(const QString&);

        QTcpSocket *tcpSocket;
        quint16 blockSize;

    private slots:
        void printError(QAbstractSocket::SocketError);
        void printReadData();
        void getMap();
        void loadMap();

    signals:
        void connected();
        void error(const QString&);
        void mapReady(const QStringList&);
};

#endif // GISCLIENT_H
