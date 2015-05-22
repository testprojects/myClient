#include <QApplication>
#include <QTimer>
#include <QtCore/QTextCodec>
#include "form.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    // for using settings in app
    QCoreApplication::setOrganizationName("GVC");
    QCoreApplication::setApplicationName("myClient");

    Form form;
    form.show();
    return a.exec();
}
