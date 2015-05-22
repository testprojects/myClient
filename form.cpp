#include "form.h"
#include "ui_form.h"
#include "client.h"
#include "packet.h"
#include "widgetplanstream.h"
#include "dialogstreamoffset.h"
#include "dialogf2.h"
#include "document.h"
#include "streamsdialog.h"
#include "settingsdialog.h"

#include <QtWidgets>

const QString SERVER_IP = "127.0.0.1";
const quint16 SERVER_PORT = 1535;
const int STRETCH = 0;

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    m_layoutProgress = new QVBoxLayout(this);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_labelStreamsPlanned = new QLabel(this);
    m_labelStreamsPlanned->setVisible(false);

    m_progressBar->setFixedWidth(width());
    m_progressBar->setRange(0, 1000);
    m_layoutProgress->addSpacing(height() - STRETCH * 2 - m_progressBar->height() - m_labelStreamsPlanned->height());
    m_layoutProgress->addWidget(m_progressBar);
    m_layoutProgress->addWidget(m_labelStreamsPlanned);
    setLayout(m_layoutProgress);

    ui->setupUi(this);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonLoadRequestDikon->setEnabled(false);
    ui->pushButtonLoadRequestZhenya->setEnabled(false);
    ui->pushButtonGetF2->setEnabled(false);
    m_client = new Client(SERVER_IP, SERVER_PORT, this);
    connect(m_client, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_client, SIGNAL(stringRecieved(QString)), this, SLOT(displayMessage(QString)));

    connect(m_client, SIGNAL(signalPlanStarted()), this, SLOT(slotPlanStarted()));
    connect(m_client, SIGNAL(signalPlanned(int,int)), this, SLOT(slotStreamPlanned(int,int)));
    connect(m_client, SIGNAL(signalStreamsFailed(int)), this, SLOT(slotStreamsFailed(int)));
    connect(m_client, SIGNAL(signalPlanFinished()), this, SLOT(slotPlanFinished()));
    connect(m_client, SIGNAL(signalOffsetStream(QString, QString, int, int)), this, SLOT(slotOffsetStream(QString, QString, int, int)));
    connect(m_client, SIGNAL(signalF2Ready(QByteArray&)), this, SLOT(createDocument(QByteArray&)));

    connect(m_client, SIGNAL(signalPausePlanning()), this, SLOT(slotPausePlanning()));
    connect(m_client, SIGNAL(signalContinuePlanning()), this, SLOT(slotContinuePlanning()));
    connect(m_client, SIGNAL(signalAbortPlanning(bool)), this, SLOT(slotAbortPlanning(bool)));

    connect(ui->streamsButton, SIGNAL(clicked()), this, SLOT(showStreamsDialog()));
    connect(ui->settingsButton, SIGNAL(clicked()), this, SLOT(showSettingsDialog()));
}

Form::~Form()
{
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
    ui->pushButtonGetF2->setEnabled(true);
    ui->streamsButton->setEnabled(true);
}

void Form::onDisconnected()
{
    ui->pushButtonConnect->setText("Connect");
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonSendRequest->setEnabled(false);
    ui->pushButtonLoadRequestDikon->setEnabled(false);
    ui->pushButtonLoadRequestZhenya->setEnabled(false);
    ui->pushButtonGetF2->setEnabled(false);
    ui->streamsButton->setEnabled(false);

    QMessageBox::information(this, "Разрыв соединения", "Сервер недоступен");
    m_progressBar->hide();
    m_labelStreamsPlanned->hide();
}

void Form::slotPausePlanning() {
    QString message = QString::fromUtf8("PAUSE_PLANNING");
    m_client->sendMessage(message);
}

void Form::slotContinuePlanning() {
    QString message = QString::fromUtf8("CONTINUE_PLANNING");
    m_client->sendMessage(message);
}

void Form::slotAbortPlanning(bool bSavePlannedThreads) {
    QString message = QString::fromUtf8("ABORT_PLANNING,%1")
            .arg(bSavePlannedThreads? "YES": "NO");
    m_client->sendMessage(message);
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

void Form::displayMessage(const QString &message)
{
    qDebug() << "displayMessage: " << message;
}

void Form::on_pushButtonLoadRequestDikon_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Дикон)", QDir::home().path(), "Text *.txt");

    if(!filePath.isEmpty()) {
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QString data;
        data = file.readAll();
        QString strCommand = QString("%1,%2").arg(LOAD_REQUEST_DIKON).arg(data);
        m_client->sendMessage(strCommand);
    }
}

void Form::on_pushButtonLoadRequestZhenya_clicked()
{
    QString filePath =  QFileDialog::getOpenFileName(this, "Загрузить заявку (Женя)", QDir::home().path(), "*.fps");
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QString data;
        data = file.readAll();
        QString strCommand = QString("%1,%2").arg(LOAD_REQUEST_ZHENYA).arg(data);
        m_client->sendMessage(strCommand);
    }
}

void Form::slotPlanStarted()
{
    m_progressBar->setVisible(true);
    m_labelStreamsPlanned->setText("COUNT / TOTAL");
    m_labelStreamsPlanned->setVisible(true);
}

void Form::slotStreamPlanned(int count, int amount)
{
    if(m_progressBar) {
        double val = (double)count / (double)amount * 1000.0;
        m_progressBar->setValue((int)val);
        m_labelStreamsPlanned->setText(QString("%1/%2").arg(count).arg(amount));
    }
}

void Form::slotStreamsFailed(int count)
{
    QMessageBox::warning(this, "Не спланированные потоки", QString("Количество: %1").arg(count));
}

void Form::slotPlanFinished()
{
    QMessageBox::information(this, "Планирование завершено", "Планирование завершено");
    m_progressBar->hide();
    m_labelStreamsPlanned->hide();
}

void Form::slotOffsetStream(QString strPassedStations, QString strOriginalDepartureTime, int NP, int hours)
{
    DialogStreamOffset dialog(strPassedStations, strOriginalDepartureTime, NP, hours);
    int ok = dialog.exec();
    bool b_Accept = (bool)ok;
    QString message = QString("%1,%2")
            .arg(ACCEPT_OFFSET)
            .arg(b_Accept);
    m_client->sendMessage(message);
}

void Form::on_pushButtonGetF2_clicked()
{
    DialogF2 dialog;
    int res = dialog.exec();
    if(res == QDialog::Accepted) {
        //формируем запрос на составление сервером формы Ф2
        QString message(QString("%1,%2,%3,%4,%5,%6,%7")
                 .arg(GET_F2)
                 .arg(dialog.VP)
                 .arg(dialog.KP_Start)
                 .arg(dialog.KP_End)
                 .arg(dialog.NP_Start)
                 .arg(dialog.NP_End)
                 .arg(dialog.grif)
                 );
        message += QString(",%1,%2,%3,%4")
                .arg(dialog.devideByKG)
                .arg(dialog.devideByOKR)
                .arg(dialog.actionOKR)
                .arg(dialog.okr);
        m_client->sendMessage(message);
    }
}

void Form::createDocument(QByteArray &ba)
{
    Document doc;
    doc.create(ba);
}

void Form::showStreamsDialog()
{
    StreamsDialog streamsDialog(this, m_client);
    streamsDialog.exec();
}

void Form::showSettingsDialog()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}
