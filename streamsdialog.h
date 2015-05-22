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
        QProcess *process;

    private slots:
        void getStreams(QByteArray&);
        void displayStream();
        void sendRequest();
        void enableButton();
        void connectToGis();
        void loadMap();
        void runGis();
        void showMessage(const QString&);
//        void removeData();
};

#endif // STREAMSDIALOG_H
