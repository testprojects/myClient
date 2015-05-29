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

    // control input data
    ui->serverIPLineEdit->setValidator(new QRegExpValidator(QRegExp("^(?:(?:25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d\\d?)$")));
    ui->serverPortLineEdit->setValidator(new QIntValidator(0, 9999));

    readSettings();

    connect(ui->saveButton, SIGNAL(clicked()), SLOT(writeSettings()));
    connect(ui->saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
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
        settings.beginGroup("GisServer");
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

    settings.beginGroup("GisServer");
    ui->serverIPLineEdit->setText(settings.value("serverIP").toString());
    ui->serverPortLineEdit->setText(settings.value("serverPort").toString());
    settings.endGroup();
}
