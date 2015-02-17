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
