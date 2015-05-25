#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    readSettings();

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(writeSettings()));
    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->reviewProgramButton, SIGNAL(clicked()), SLOT(reviewProgram()));
    connect(ui->reviewMapButton, SIGNAL(clicked()), SLOT(reviewMap()));
}

SettingsDialog::~SettingsDialog()
{
    writeSettings();
    delete ui;
}

void SettingsDialog::writeSettings()
{
    QSettings settings;

    settings.beginGroup(QString("Geometry/%1").arg(this->windowTitle()));
    settings.setValue("width", this->width());
    settings.setValue("height", this->height());
    settings.endGroup();

    if (sender() && sender()->objectName().contains("saveButton")) {
        settings.beginGroup("Gis");
        settings.setValue("pathToProgram", ui->pathToProgramLineEdit->text().simplified());
        settings.setValue("pathToMap", ui->pathToMapLineEdit->text().simplified());
        settings.endGroup();

        settings.beginGroup("Server");
        settings.setValue("serverIP", ui->serverIPLineEdit->text());
        settings.setValue("serverPort", ui->serverPortLineEdit->text());
        settings.endGroup();
    }
}

void SettingsDialog::readSettings()
{
    QSettings settings;

    settings.beginGroup(QString("Geometry/%1").arg(this->windowTitle()));
    int width = settings.value("width", 400).toInt();
    int height = settings.value("height", 300).toInt();
    this->resize(width, height);
    settings.endGroup();

    settings.beginGroup("Gis");
    ui->pathToProgramLineEdit->setText(QDir::toNativeSeparators(settings.value("pathToProgram", "").toString()));
    ui->pathToMapLineEdit->setText(QDir::toNativeSeparators(settings.value("pathToMap", "").toString()));
    settings.endGroup();

    settings.beginGroup("Server");
    ui->serverIPLineEdit->setText(settings.value("serverIP").toString());
    ui->serverPortLineEdit->setText(settings.value("serverPort").toString());
    settings.endGroup();
}

void SettingsDialog::reviewProgram()
{
    QString pathToProgram = QFileDialog::getOpenFileName(this, tr("Select Gis program"), (ui->pathToProgramLineEdit->text().isEmpty() ? QDir::homePath() :
                                                         QFileInfo(ui->pathToProgramLineEdit->text()).absoluteFilePath()), tr("Gis program (*.exe)"));

    if (pathToProgram.isEmpty())
        return;

    ui->pathToProgramLineEdit->setText(QDir::toNativeSeparators(pathToProgram));
}

void SettingsDialog::reviewMap()
{
    QString pathToMap = QFileDialog::getOpenFileName(this, tr("Select map file"), (ui->pathToMapLineEdit->text().isEmpty() ? QDir::homePath() :
                                                         QFileInfo(ui->pathToMapLineEdit->text()).absoluteFilePath()), tr("Map file (*.map)"));

    if (pathToMap.isEmpty())
        return;

    ui->pathToMapLineEdit->setText(QDir::toNativeSeparators(pathToMap));
}
