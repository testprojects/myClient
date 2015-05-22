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
    process = 0;

    sendRequest();

    connect(this->client, SIGNAL(signalStreamsReady(QByteArray&)), this, SLOT(getStreams(QByteArray&)));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->displayStreamButton, SIGNAL(clicked()), this, SLOT(displayStream()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(sendRequest()));
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(enableButton()));
    connect(ui->openGisButton, SIGNAL(clicked()), SLOT(runGis()));
}

StreamsDialog::~StreamsDialog()
{
    delete ui;
}

void StreamsDialog::sendRequest()
{
    client->sendMessage(QString("%1").arg(GET_STREAMS));
}

void StreamsDialog::displayStream()
{
    if (!selectionModel->hasSelection())
        return;

    QSettings settings;

    //    gisClient->createObject();
    int curRow = selectionModel->currentIndex().row();
    client->sendMessage(QString("%1,%2,%3,%4,%5")
                        .arg(DISPLAY_STREAM)
                        .arg(streamsModel->item(curRow, 0)->text())
                        .arg(streamsModel->item(curRow, 1)->text())
                        .arg(streamsModel->item(curRow, 2)->text())
                        .arg(settings.value("Gis/pathToMap").toString()));
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
    ui->displayStreamButton->setEnabled((process && process->state()) && selectionModel->hasSelection());
    ui->openGisButton->setEnabled((process && !process->state()) || (!process));
}

void StreamsDialog::runGis()
{
    if (process && process->state() == QProcess::Running)
        return;

    QSettings settings;
    QFileInfo fileInfo(settings.value("Gis/pathToProgram").toString());

    process = new QProcess(this);
    connect(process, SIGNAL(started()), this, SLOT(connectToGis()));
    connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), SLOT(enableButton()));
//    connect(process, SIGNAL(finished(int)), this, SLOT(removeData()));

    QDir::setCurrent(fileInfo.absolutePath());
    process->start(fileInfo.fileName(), QStringList() << "/P");      // argument P = enable transport data
//    return process->waitForStarted();
}

void StreamsDialog::connectToGis()
{
    gisClient = new GisClient(this);

    connect(gisClient, SIGNAL(connected()), SLOT(loadMap()));
    connect(gisClient, SIGNAL(error(QString)), SLOT(showMessage(QString)));
    connect(gisClient, SIGNAL(error(QString)), process, SLOT(kill()));
}

void StreamsDialog::loadMap()
{
    gisClient->loadMap();
}

//void StreamsDialog::removeData()
//{
//    gisClient->removeData();
//}

void StreamsDialog::showMessage(const QString &message)
{
    QMessageBox::warning(this, tr("Error"), message);
}
