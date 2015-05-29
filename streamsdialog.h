#ifndef STREAMSDIALOG_H
#define STREAMSDIALOG_H

#include <QDialog>

namespace Ui {
class StreamsDialog;
}

class QStandardItemModel;
class QItemSelectionModel;
class Client;
class QProcess;
class GisClient;
class StreamsDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit StreamsDialog(QWidget *parent, Client *client);
        ~StreamsDialog();

    private:
        Ui::StreamsDialog *ui;
        QStandardItemModel *streamsModel;
        QItemSelectionModel *selectionModel;
        Client *client;
        GisClient *gisClient;

    private slots:
        void getStreams(QByteArray&);
        void getStreamCoordinats();
        void displayStream(QByteArray&);
        void sendRequest();
        void enableButton();
        void showMessage(const QString&);
        void showListMaps(const QStringList&);
};

#endif // STREAMSDIALOG_H
