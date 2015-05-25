#include "connecttoserverdialog.h"
#include "ui_connecttoserverdialog.h"
#include <QSettings>

connectToServerDialog::connectToServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connectToServerDialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("Server");
    m_serverIP = settings.value("serverIP").toString();
    m_serverPort = settings.value("serverPort").toString();
    settings.endGroup();

    if(m_serverIP.isEmpty()) {
        m_serverIP = SERVER_IP;
    }
    if(m_serverPort.isEmpty()) {
        m_serverPort = QString::number(SERVER_PORT);
    }
    ui->lineEditIP->setText(m_serverIP);
    ui->lineEditPort->setText(m_serverPort);
}

connectToServerDialog::~connectToServerDialog()
{
    delete ui;
}

void connectToServerDialog::on_pushButtonConnect_clicked()
{
    m_serverIP = ui->lineEditIP->text();
    m_serverPort = ui->lineEditPort->text();
    QSettings settings;
    settings.beginGroup("Server");
    settings.setValue("serverIP", m_serverIP);
    settings.setValue("serverPort", m_serverPort);
    settings.endGroup();

    accept();
}

void connectToServerDialog::on_pushButtonCancel_clicked()
{
    reject();
}

QString connectToServerDialog::serverIP() {
    return m_serverIP;
}

QString connectToServerDialog::serverPort() {
    return m_serverPort;
}
