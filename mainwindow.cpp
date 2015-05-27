#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include "packet.h"
#include "widgetplanstream.h"
#include "dialogstreamoffset.h"
#include "dialogf2.h"
#include "document.h"
#include "streamsdialog.h"
#include "settingsdialog.h"
#include "assert.h"
#include "connecttoserverdialog.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_state(NOT_PLANNING)
{
    ui->setupUi(this);

    ui->toolButtonPause->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->labelStreams->setVisible(false);

    onConnectedDisableInterface();

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
    connect(m_client, SIGNAL(signalContinuePlanning()), this, SLOT(slotResumePlanning()));
    connect(m_client, SIGNAL(signalAbortPlanning(bool)), this, SLOT(slotAbortPlanning(bool)));

    connect(m_client, SIGNAL(signalPlanPaused()), this, SLOT(slotPlanPaused()));
    connect(m_client, SIGNAL(signalPlanResumed()), this, SLOT(slotPlanResumed()));
    connect(m_client, SIGNAL(signalPlanAborted()), this, SLOT(slotPlanAborted()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectedEnableInterface() {
    ui->actionConnect->setText("Отключиться от сервера");
    ui->actionConnect->setEnabled(true);
    ui->actionConnect->setIcon(QIcon(":/resources/icon_disconnected_128.png"));

    ui->pushButtonSendRequest->setEnabled(true);
    ui->actionDBLoadRequestDikon->setEnabled(true);
    ui->actionDBLoadRequestZhenya->setEnabled(true);
    ui->actionFormsF2PC->setEnabled(true);
    ui->actionDBDeleteAllStreams->setEnabled(true);
    ui->actionCacheOut->setEnabled(true);
    ui->actionDisplayStreams->setEnabled(true);
}

void MainWindow::onConnectedDisableInterface() {
    ui->actionConnect->setText("Подключиться к серверу");
    ui->actionConnect->setEnabled(true);
    ui->actionConnect->setIcon(QIcon(":/resources/icon_connected_128.png"));
    ui->pushButtonSendRequest->setEnabled(false);
    ui->actionDBLoadRequestDikon->setEnabled(false);
    ui->actionDBLoadRequestZhenya->setEnabled(false);
    ui->actionFormsF2PC->setEnabled(false);
    ui->actionDBDeleteAllStreams->setEnabled(false);
    ui->actionCacheOut->setEnabled(false);
    ui->actionDisplayStreams->setEnabled(false);
}

void MainWindow::onConnected()
{
    ui->actionConnect->setText("Отключиться от сервера");
    ui->actionConnect->setEnabled(true);
    m_state = NOT_PLANNING;
    ui->statusbar->showMessage("Соединение установлено");

    onConnectedEnableInterface();
}

void MainWindow::onDisconnected()
{
    onConnectedDisableInterface();
    QMessageBox::information(this, "Разрыв соединения", "Сервер недоступен");
    m_state = NOT_PLANNING;
    ui->statusbar->showMessage("Соединение разорвано");
    ui->progressBar->hide();
    ui->labelStreams->hide();
    ui->toolButtonPause->hide();
}

void MainWindow::slotPausePlanning() {
    qDebug() << "MainwWindow::slotPausePlanning()";
    ui->toolButtonPause->setVisible(true);
    ui->labelStreams->setVisible(true);
    ui->progressBar->setVisible(true);
    QString message = QString::fromUtf8("%1").arg(PAUSE_PLANNING);
    m_client->sendMessage(message);
}

void MainWindow::slotResumePlanning() {
    QString message = QString::fromUtf8("%1").arg(CONTINUE_PLANNING);
    m_client->sendMessage(message);
}

void MainWindow::slotAbortPlanning(bool bSavePlannedThreads) {
    QString message = QString::fromUtf8("%1,%2")
            .arg(ABORT_PLANNING)
            .arg(bSavePlannedThreads? "YES": "NO");
    m_client->sendMessage(message);
}

void MainWindow::on_pushButtonSendRequest_clicked()
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

void MainWindow::displayMessage(const QString &message)
{
    qDebug() << "displayMessage: " << message;
}

void MainWindow::slotPlanStarted()
{
    m_state = PLANNING;
    ui->progressBar->setMaximum(1000);
    ui->progressBar->setVisible(true);
    ui->labelStreams->setText("COUNT / TOTAL");
    ui->labelStreams->setVisible(true);
    ui->toolButtonPause->setIcon(QIcon(QString(":/resources/icon_pause_128.png")));
    ui->toolButtonPause->setVisible(true);
    ui->statusbar->showMessage("Планирование потоков...");
}

void MainWindow::slotPlanFinished()
{
    m_state = NOT_PLANNING;
    ui->statusbar->showMessage("Готово");
    ui->progressBar->hide();
    ui->labelStreams->hide();
    ui->toolButtonPause->hide();
}

void MainWindow::slotPlanPaused() {
    ui->toolButtonPause->setIcon(QIcon(":/resources/icon_resume_128"));
}

void MainWindow::slotPlanResumed() {
    ui->toolButtonPause->setIcon(QIcon(":/resources/icon_pause_128"));
}

void MainWindow::slotPlanAborted() {
    //может стоит добавить что-то ещё для того, чтобы различать незавершённое планирование
    slotPlanFinished();
}

void MainWindow::slotStreamPlanned(int count, int amount)
{
    double val = (double)count / (double)amount * 1000.0;
    ui->progressBar->setValue((int)val);
    ui->labelStreams->setText(QString("%1/%2").arg(count).arg(amount));
}

void MainWindow::slotStreamsFailed(int count)
{
    QMessageBox::warning(this, "Не спланированные потоки", QString("Количество: %1").arg(count));
}



void MainWindow::slotOffsetStream(QString strPassedStations, QString strOriginalDepartureTime, int NP, int hours)
{
    DialogStreamOffset dialog(strPassedStations, strOriginalDepartureTime, NP, hours);
    int ok = dialog.exec();
    bool b_Accept = (bool)ok;
    QString message = QString("%1,%2")
            .arg(ACCEPT_OFFSET)
            .arg(b_Accept);
    m_client->sendMessage(message);
}


void MainWindow::createDocument(QByteArray &ba)
{
    Document doc;
    doc.create(ba);
}

void MainWindow::on_actionDBLoadRequestDikon_triggered()
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

void MainWindow::on_actionDBLoadRequestZhenya_triggered()
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

void MainWindow::on_actionDBDeleteAllStreams_triggered()
{
    //вызов метода MyDB
    m_client->sendMessage(QString::number(DB_REMOVE_ALL_STREAMS));
}

void MainWindow::on_actionFormsF2PC_triggered()
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

void MainWindow::on_actionSettings_triggered() {
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}

void MainWindow::on_actionConnect_triggered()
{
    if(!m_client->isConnected()) {
        connectToServerDialog dialog;
        int res = dialog.exec();
        if(res == QDialog::Accepted) {
            m_client->setServerIP(dialog.serverIP());
            m_client->setServerPort(dialog.serverPort().toUInt());
            ui->actionConnect->setEnabled(false);
            m_client->connectToServer();
        }
    }
    else {
        m_client->disconnectFromServer();
    }
}
void MainWindow::on_actionCacheOut_triggered()
{
    m_client->sendMessage(QString::number(CACHE_OUT));
}

void MainWindow::on_actionDisplayStreams_triggered()
{
    StreamsDialog streamsDialog(this, m_client);
    streamsDialog.exec();
}

void MainWindow::on_actionActionPauseResume_triggered()
{
    if(m_state == PLANNING)
        slotPausePlanning();
    else if(m_state == PAUSED)
        slotResumePlanning();
    else if(m_state == NOT_PLANNING) {
        qDebug() << "how could be paused/resumed, while not planning?";
        return;
    }
}
