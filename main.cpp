#include <QApplication>
#include <QTimer>
#include <QtCore/QTextCodec>
#include "form.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    Form form;
    form.show();
    return a.exec();
}
