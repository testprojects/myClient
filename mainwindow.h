//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "client.h"

namespace Ui {
class MainWindow;
}

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

private slots:
    void on_pushButtonSendRequest_clicked();
    void on_pushButtonLoadRequestDikon_clicked();
    void on_pushButtonLoadRequestZhenya_clicked();

private slots:
    void slotPausePlanning();
    void slotContinuePlanning();
    void slotAbortPlanning(bool bSavePlannedThreads = true);

    void slotPlanStarted();
    void slotStreamPlanned(int count, int amount);
    void slotStreamsFailed(int count);
    void slotPlanFinished();
    void slotOffsetStream(QString strPassedStations, QString strOriginalDepartureTime, int NP, int hours);
    void createDocument(QByteArray &ba);

    void on_pushButtonGetF2_clicked();
    void showStreamsDialog();
    void showSettingsDialog();

    void on_pushButtonConnect_clicked();

private:
    Client *m_client;
    Ui::MainWindow *ui;

    QProgressBar *m_progressBar;
    QLabel *m_labelStreamsPlanned;
    QVBoxLayout *m_layoutProgress;

};

