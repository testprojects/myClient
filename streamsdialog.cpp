#include "streamsdialog.h"
#include "ui_streamsdialog.h"

#include "client.h"
#include <QDebug>
#include <QXmlStreamReader>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>
#include <QtCore/QProcess>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "gisclient.h"

StreamsDialog::StreamsDialog(QWidget *parent, Client *client) :
    QDialog(parent),
    ui(new Ui::StreamsDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->client = client;
    streamsModel = new QStandardItemModel();
    selectionModel = new QItemSelectionModel(streamsModel);
    ui->tableView->setModel(streamsModel);
    ui->tableView->setSelectionModel(selectionModel);

    sendRequest();

    gisClient = new GisClient(this);

    connect(this->client, SIGNAL(signalStreamsReady(QByteArray&)), this, SLOT(getStreams(QByteArray&)));
    connect(this->client, SIGNAL(signalStreamCoordinatsReady(QByteArray&)), this, SLOT(displayStream(QByteArray&)));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->displayStreamButton, SIGNAL(clicked()), this, SLOT(getStreamCoordinats()));
    connect(ui->displayAllStreamsButton, SIGNAL(clicked()), this, SLOT(getStreamCoordinats()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(sendRequest()));
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(enableButton()));
    connect(streamsModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(enableButton()));
    connect(ui->selectMapButton, SIGNAL(clicked()), gisClient, SLOT(getMap()));
    connect(ui->loadMapButton, SIGNAL(clicked()), gisClient, SLOT(loadMap()));
    connect(gisClient, SIGNAL(error(QString)), SLOT(showMessage(QString)));
    connect(gisClient, SIGNAL(mapReady(QStringList)), SLOT(showListMaps(QStringList)));
}

StreamsDialog::~StreamsDialog()
{
    delete ui;
}

void StreamsDialog::sendRequest()
{
    client->sendMessage(QString("%1").arg(GET_STREAMS));
}

void StreamsDialog::getStreamCoordinats()
{
    QString message = QString();

    // get coordinats stream
    if (selectionModel->hasSelection() && sender()->objectName().contains("displayStreamButton")) {
        int curRow = selectionModel->currentIndex().row();
        message = QString(",%1,%2,%3")
                .arg(streamsModel->item(curRow, 0)->text())
                .arg(streamsModel->item(curRow, 1)->text())
                .arg(streamsModel->item(curRow, 2)->text());
    }

    client->sendMessage(QString("%1").arg(DISPLAY_STREAM) + message);
}

void StreamsDialog::displayStream(QByteArray &streamCoordinats)
{
    QXmlStreamReader xmlReader(streamCoordinats);
    int VP;
    int KP;
    int NP;
    QStringList coordinats = QStringList();

    do {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
            if (xmlReader.name() == "VP")
                VP = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "KP")
                KP = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "NP")
                NP = xmlReader.readElementText().toInt();
            else if (xmlReader.name() == "coordinats")
                coordinats << xmlReader.readElementText();
        }
        else if (xmlReader.isEndElement())
            if (xmlReader.name() == "stream") {
                gisClient->createObject(VP, KP, NP, coordinats);

                coordinats.clear();
            }
    } while(!xmlReader.atEnd());

    if (xmlReader.hasError()) {
        qDebug() << "Parsing XML: " << xmlReader.errorString();
        return;
    }
}

void StreamsDialog::getStreams(QByteArray &streams)
{
    QXmlStreamReader xmlReader(streams);
    QList<QStandardItem*> items = QList<QStandardItem*>();
    QStringList headerTable = QStringList();

    streamsModel->clear();
    do {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
            if (xmlReader.name() != "document" && xmlReader.name() != "stream") {
                // get name column
                if (!headerTable.contains(xmlReader.name().toString()))
                    headerTable.append(xmlReader.name().toString());

                items.append(new QStandardItem(xmlReader.readElementText()));
            }
        }
        else if (xmlReader.isEndElement())
            if (xmlReader.name() == "stream") {
                streamsModel->appendRow(items);
                items.clear();
            }
    } while(!xmlReader.atEnd());

    if (xmlReader.hasError()) {
        qDebug() << "Parsing XML: " << xmlReader.errorString();
        return;
    }

    streamsModel->setHorizontalHeaderLabels(headerTable);
}

void StreamsDialog::enableButton()
{
    ui->displayStreamButton->setEnabled(selectionModel->hasSelection());
    ui->displayAllStreamsButton->setEnabled(streamsModel->rowCount() > 0);
}

void StreamsDialog::showMessage(const QString &message)
{
    QMessageBox::warning(this, tr("Error"), message);
}

void StreamsDialog::showListMaps(const QStringList &maps)
{
    bool ok;
    QString pathToMap = QInputDialog::getItem(this, tr("Select map"), tr("Map:"), maps, 0, false, &ok);

    if (ok && !pathToMap.isEmpty()) {
        QSettings settings;
        settings.setValue("GisServer/map", pathToMap);
    }
}
