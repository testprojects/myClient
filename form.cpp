#include "form.h"
#include "ui_form.h"
#include "client.h"

#include <QtWidgets>

const QString SERVER_IP = "127.0.0.1";
const quint16 SERVER_PORT = 1535;

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->pushButtonSendRequest->setEnabled(false);
    m_client = new Client(SERVER_IP, SERVER_PORT, this);
    connect(m_client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_client, SIGNAL(messageRecieved(QString)), this, SLOT(displayMessage(QString)));
}

Form::~Form()
{
    connect(m_client, SIGNAL(disconnected()), SLOT(deleteLater()));
    m_client->disconnectFromServer();
    delete ui;
}

void Form::on_pushButtonConnect_clicked()
{
    if(ui->pushButtonConnect->text() == "Connect") {
        ui->pushButtonConnect->setEnabled(false);
        m_client->connectToServer();
    }
    else {
        ui->pushButtonConnect->setEnabled(false);
        m_client->disconnectFromServer();
    }
}

void Form::onConnected()
{
    ui->pushButtonConnect->setText("Disconnect");
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonSendRequest->setEnabled(true);
}

void Form::onDisconnected()
{
    ui->pushButtonConnect->setText("Connect");
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonSendRequest->setEnabled(false);
}


void Form::on_pushButtonSendRequest_clicked()
{
    m_client->sendMessage("PLAN_STREAM(XXX)");
}

void Form::displayMessage(QString message)
{
    QMessageBox::information(this, "Information", message);
}

void Form::on_pushButtonLoadRequestDikon_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Дикон)", QDir::home().path(), "Text *.txt");
    if(!filePath.isEmpty())
        m_client->sendMessage(QString("LOAD_REQUEST(%1)").arg(filePath));
}

void Form::on_pushButtonLoadRequestZhenya_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Женя)", QDir::home().path(), "*.fps");
    if(!filePath.isEmpty())
        m_client->sendMessage(QString("LOAD_REQUEST(%1)").arg(filePath));
}
