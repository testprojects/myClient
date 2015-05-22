#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QStringList>
#include "packet.h"
class QTcpSocket;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QString serverIP, quint16 serverPort, QObject *parent = 0);
    ~Client();

public slots:
    //отсылаем сообщение серверу
    void sendMessage(const QString &message);
    //читаем сообщение от сервера
    void readPacket();
    //подключены ли мы к серверу (не всегда правда)

    bool isConnected() const;
    //пытаемся присоединиться к серверу через установленный интервал времени
    void connectToServer();
    //отсоединяемся
    void disconnectFromServer();

signals:
    //сигналы от сокета переадресовываем классу интерфейса
    void connected();
    void disconnected();
    void stringRecieved(QString);
private slots:
    //слот, который пытается подключиться через интервалы времени
    void tryToConnect();
    void dispatchMessage(QString message);

signals:
    void signalPlanStarted();
    void signalPlanned(int count, int amount);
    void signalStreamsFailed(int count);
    void signalPlanFinished();
    void signalOffsetStream(int VP, int KP, int NP, int hours);
    void signalF2Ready(QByteArray &ba);
    void signalStreamsReady(QByteArray &ba);

private:
    //сокет на запись
    QTcpSocket *m_tcpSocket;
public:
    QString m_serverIP;
    quint16 m_serverPort;
    quint32 m_blockSize;
};

#endif // CLIENT_H
