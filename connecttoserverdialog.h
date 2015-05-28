#ifndef CONNECTTOSERVERDIALOG_H
#define CONNECTTOSERVERDIALOG_H

#include <QDialog>
const QString SERVER_IP = "127.0.0.1";
const quint16 SERVER_PORT = 1535;

namespace Ui {
class connectToServerDialog;
}

class connectToServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectToServerDialog(QWidget *parent = 0);
    ~connectToServerDialog();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::connectToServerDialog *ui;
    QString m_serverIP;
    QString m_serverPort;

public:
    QString serverIP();
    QString serverPort();
};

#endif // CONNECTTOSERVERDIALOG_H
