#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QStringList>
#include "packet.h"
class QTcpSocket;
class QTimer;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QString serverIP, quint16 serverPort, QObject *parent = 0);
    ~Client();

    void setServerIP(QString serverIP);
    QString serverIP();
    void setServerPort(quint16 port);
    quint16 serverPort();

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
    void signalFailedConnection();
    void stringRecieved(QString);
private slots:
    void dispatchMessage(QString message);

signals:
    void signalPausePlanning();
    void signalAbortPlanning(bool bSavePlannedStreams = true);
    void signalContinuePlanning();

    void signalPlanPaused();
    void signalPlanResumed();
    void signalPlanAborted();


    void signalPlanStarted();
    void signalPlanned(int count, int amount);
    void signalStreamsFailed(int count);
    void signalPlanFinished();
    void signalOffsetStream(QString strPassedStations, QString strDepartureTime, int NP, int hours);
    void signalF2Ready(QByteArray &ba);
    void signalStreamsReady(QByteArray &ba);

private:
    //сокет на запись
    QTcpSocket *m_tcpSocket;
    QTime *obstacleTime;
    QTimer *connectTimer;
public:
    QString m_serverIP;
    quint16 m_serverPort;
    quint32 m_blockSize;
};

#endif // CLIENT_H
