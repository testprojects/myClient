#include <QApplication>
#include <QTimer>
#include "form.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Form form;
    form.show();
    return a.exec();
}
