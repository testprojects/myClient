//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "client.h"

namespace Ui {
class MainWindow;
}

enum planning_state{PLANNING, PAUSED, NOT_PLANNING};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void displayMessage(const QString &message);
    void onConnected();
    void onDisconnected();
    void onConnectedDisableInterface();
    void onConnectedEnableInterface();

private slots:
    void on_pushButtonSendRequest_clicked();

private slots:
    void slotPausePlanning();
    void slotResumePlanning();
    void slotAbortPlanning(bool bSavePlannedThreads = true);


    void slotPlanStarted();
    void slotPlanFinished();
    void slotPlanPaused();
    void slotPlanResumed();
    void slotPlanAborted();

    void slotStreamPlanned(int count, int amount);
    void slotStreamsFailed(int count);
    void slotOffsetStream(QString strPassedStations, QString strOriginalDepartureTime, int NP, int hours);
    void createDocument(QByteArray &ba);

    void on_actionDBLoadRequestDikon_triggered();

    void on_actionDBLoadRequestZhenya_triggered();

    void on_actionDBDeleteAllStreams_triggered();

    void on_actionFormsF2PC_triggered();

    void on_actionConnect_triggered();

    void on_actionSettings_triggered();

    void on_actionCacheOut_triggered();

    void on_actionDisplayStreams_triggered();

    void on_actionActionPauseResume_triggered();

private:
    Client *m_client;
    Ui::MainWindow *ui;
    planning_state m_state;
};

