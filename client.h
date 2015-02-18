#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
class QTcpSocket;

enum Commands {PLAN_SUZ = 1, PLAN_BUZ, CHANGE_TO, CACHE_OUT, LOAD_REQUEST_DIKON, LOAD_REQUEST_ZHENYA};
struct Package {
    int len;            //размер структуры
    Commands command;   //команда на выполнение
    QStringList params; //параметры команды
};

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QString serverIP, quint16 serverPort, QObject *parent = 0);
    ~Client();

public slots:
    //отсылаем сообщение серверу
    void sendMessage(QString message);
    //читаем сообщение от сервера
    void readMessage();
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
    void messageRecieved(QString);

private slots:
    //слот, который пытается подключиться через интервалы времени
    void tryToConnect();

private:
    //сокет на запись
    QTcpSocket *m_tcpSocket;

public:
    QString m_serverIP;
    quint16 m_serverPort;
public:
    QString m_serverMessage;
};

#endif // CLIENT_H
