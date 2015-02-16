#ifndef CLIENT_H
#define CLIENT_H
class QTcpSocket;

//Клиент взаимодействует с сервером по двум соединениям
//1. Соединение на передачу (Передаёт команды интерфейса на выполнение расчётов, Изменение объектов БД)
//2. Соединение на приём (Принимает от сервера сигналы об успешном/неуспешном выполнении расчётов, Выходные документы,
//Данные по объектам, которые нужно изменить
//Подключение считается установленным, если установлено оба соединения
//Подключение считается разорванным, если разорвано хотя бы одно из соединений


#include <QObject>

class Client : public QObject
{
    Q_OBJECT
public:
    Client();
    ~Client();

public slots:
    //отсылаем сообщение серверу
    void sendMessage(QString message);
    //читаем сообщение от сервера
    void readMessage();
    //подключены ли мы к серверу (не всегда правда)
    bool isConnected() const;
    //пытаемся присоединиться к серверу через установленный интервал времени
    void connectToServer(QString serverIP, quint16 serverPort);
    //отсоединяемся
    void disconnectFromServer();

signals:
    //сигналы от сокета переадресовываем классу интерфейса
    void connected();
    void disconnected();

    void connectedOut();
    void connectedIn();
    void disconnectedOut();
    void disconnectedIn();

private slots:
    //слот, который пытается подключиться через интервалы времени
    void tryToConnect();
    void dispatchServerMessage();
    void emitConnect();
    void emitDisconnect();

private:
    //сокет на запись
    QTcpSocket *m_tcpSocketOut;
    //сокет на чтение
    QTcpSocket *m_tcpSocketIn;

    QString m_serverIP;
    quint16 m_serverPort;
    QString m_serverMessage;
};

#endif // CLIENT_H
