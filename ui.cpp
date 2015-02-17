#include "ui.h"
#include "client.h"
#include <iostream>
#include <QCoreApplication>
#include <QTimer>

enum subMenus {MENU_PLANNING = 1, MENU_PLANNING_STREAM = 11, MENU_WORKWITHDATABASE = 2, MENU_WORKWITHDATABASE_LOADREQUEST = 21};

UI::UI(QObject *parent) : QObject(parent), m_client(0)
{
}

void UI::connectToServer(QString serverIP, quint16 serverPort)
{
//    m_client = new Client(this);
    connect(m_client, SIGNAL(connected()), this, SLOT(showSubMenu()));
    connect(m_client, SIGNAL(messageRecieved()), this, SLOT(printServerMessage()));
//    m_client->connectToServer(serverIP, serverPort);
}

void UI::disconnectFromServer()
{
    m_client->disconnectFromServer();
}

void UI::printServerMessage()
{
    std::cout << "Server message: " << m_client->m_serverMessage.toStdString();
}

void UI::showMainMenu(QString error)
{
    QString choice;
    clearScreen();
    std::string buf;
    if(!error.isEmpty())
        std::cout << error.toStdString() << std::endl;
    std::cout << "1)connect to server\n";
    std::cout << "9)exit\n";
    std::cin >> buf;
    choice = QString::fromStdString(buf);

    switch (choice.toInt()) {
    case 1:
        clearScreen();
        std::cout << "Connecting...\n";
        connectToServer("127.0.0.1", 1535);
        break;
    case 9:
        exit();
        break;
    default:
        break;
    }
}

void UI::clearScreen()
{
    std::cout << "\033[H\033[2J" << std::flush;
}

void UI::exit()
{
    clearScreen();
    std::cout << "Client exited\n";
    delete m_client;
    std::exit(0);
}

void UI::showSubMenu()
{
    QString choice;
    while(true && m_client->isConnected()) {
        clearScreen();
        std::string buf;
        std::cout << "1)Planning\n";
        std::cout << "2)Work with DataBase\n";
        std::cout << "8)Disconnect from server\n";
        std::cout << "9)exit\n";
        std::cin >> buf;
        choice = QString::fromStdString(buf);
        if(!m_client->isConnected())
            continue;


        switch (choice.toInt()) {
        case 1:
        {
            showMenuPlanning();
            break;
        }
        case 2:
        {
            showMenuWorkWithDatabase();
            break;
        }
        case 8:
        {
            disconnectFromServer();
            continue;
            break;
        }
        case 9:
            exit();
            break;
        default:
            break;
        }
    }
    showMainMenu("Disconnected from server");
}


void UI::showMenuPlanning()
{
    QString choice;
    while(true && m_client->isConnected()) {
        clearScreen();
        std::string buf;
        std::cout << "1)back\n";
        std::cout << "2)plan stream\n";
        std::cin >> buf;
        choice = QString::fromStdString(buf);

        switch (choice.toInt()) {
        case 1:
            return;
        case 2:
        {
            int VP, KP, NP;
            std::cout << "VP = "; std::cin >> VP;
            std::cout << "KP = "; std::cin >> KP;
            std::cout << "NP = "; std::cin >> NP;
            m_client->sendMessage(QString("PLAN_STREAM(%1, %2, %3)").arg(VP).arg(KP).arg(NP));
            break;
        }
        default:
            break;
        }
    }
}

void UI::showMenuWorkWithDatabase()
{
    QString choice;
    while(true && m_client->isConnected()) {
        clearScreen();
        std::string buf;
        std::cout << "1)back\n";
        std::cout << "2)load request from file\n";
        std::cin >> buf;
        choice = QString::fromStdString(buf);

        switch (choice.toInt()) {
        case 1:
            return;
        case 2:
        {
            QString filePath;
            std::string buf;
            std::cout << "filePath = "; std::cin >> buf;
            filePath=QString::fromStdString(buf);
            m_client->sendMessage(QString("LOAD_REQUEST_FROM_FILE(%1)").arg(filePath));
            break;
        }
        default:
            break;
        }
    }
}
