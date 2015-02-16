#include <QCoreApplication>
#include <QTimer>
#include "client.h"
#include "ui.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    UI *ui = new UI(&a);
    ui->showMainMenu();
    return a.exec();
}
