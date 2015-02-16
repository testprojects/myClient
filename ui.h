#ifndef UI_H
#define UI_H
#include <QObject>
class Client;

class UI: public QObject
{
    Q_OBJECT
public:
    UI(QObject *parent = 0);

public slots:
    void showMainMenu(QString error = "");
    void showSubMenu();
    void exit();

private slots:
    void connectToServer(QString serverIP, quint16 serverPort);
    void disconnectFromServer();

private:
    void sendMessage(QString message);
    void showMenuPlanning();
    void showMenuWorkWithDatabase();
    void clearScreen();

private:
    Client *m_client;
};

#endif // UI_H
