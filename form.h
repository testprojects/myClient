#ifndef FORM_H
#define FORM_H

#include <QWidget>
class Client;
class QMenu;

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private slots:
    void displayMessage(QString message);
    void on_pushButtonConnect_clicked();
    void onConnected();
    void onDisconnected();

    void on_pushButtonSendRequest_clicked();


    void on_pushButtonLoadRequestDikon_clicked();

    void on_pushButtonLoadRequestZhenya_clicked();

private:
    Ui::Form *ui;
    Client *m_client;
};

#endif // FORM_H
