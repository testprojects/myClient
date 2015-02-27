#include "form.h"
#include "ui_form.h"
#include "client.h"
#include "packet.h"
#include "widgetplanstream.h"

#include <QtWidgets>

const QString SERVER_IP = "127.0.0.1";
const quint16 SERVER_PORT = 1535;

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonLoadRequestDikon->setEnabled(false);
    ui->pushButtonLoadRequestZhenya->setEnabled(false);
    m_client = new Client(SERVER_IP, SERVER_PORT, this);
    connect(m_client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_client, SIGNAL(stringRecieved(QString)), this, SLOT(displayString(QString)));
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
    ui->pushButtonLoadRequestDikon->setEnabled(true);
    ui->pushButtonLoadRequestZhenya->setEnabled(true);
}

void Form::onDisconnected()
{
    ui->pushButtonConnect->setText("Connect");
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonLoadRequestDikon->setEnabled(false);
    ui->pushButtonLoadRequestZhenya->setEnabled(false);
}


void Form::on_pushButtonSendRequest_clicked()
{
    WidgetPlanStream widget;
    int VP, KP, NP_Start, NP_End;
    bool SUZ;
    int res = widget.exec();
    switch (res) {
    case PLAN_STREAM_CLICKED: {
        VP = widget.getVP();
        KP = widget.getKP();
        NP_Start = widget.getNP_Start();
        NP_End = widget.getNP_End();
        SUZ = widget.getSUZ();
        QString message = QString("%1,%2,%3,%4,%5")
                .arg(SUZ ? PLAN_SUZ : PLAN_BUZ)
                .arg(VP)
                .arg(KP)
                .arg(NP_Start)
                .arg(NP_End);
        m_client->sendMessage(message);
        break;
    }
    case CANCEL_CLICKED: break;
    }
}

void Form::displayPacket(Packet& pack)
{
//    QString message = QString("command:%1 params:%2")
//            .arg(pack.command)
//            .arg(pack.params.join(','));
//    QMessageBox::information(this, "Information", message);
}

void Form::displayString(QString str)
{
    QString examp = "WTF";
    QMessageBox::information(this, "String recieved", str);
}

void Form::on_pushButtonLoadRequestDikon_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Дикон)", QDir::home().path(), "Text *.txt");

    if(!filePath.isEmpty()) {
        QString strCommand = QString("LOAD_STREAM(%1)").arg(filePath);
        m_client->sendMessage(strCommand);
    }
}

void Form::on_pushButtonLoadRequestZhenya_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Женя)", QDir::home().path(), "*.fps");
    if(!filePath.isEmpty()) {
        QString strCommand = QString("LOAD_STREAM(%1)").arg(filePath);
        m_client->sendMessage(strCommand);
    }
}
