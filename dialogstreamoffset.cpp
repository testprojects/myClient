#include "dialogstreamoffset.h"
#include "ui_dialogstreamoffset.h"

DialogStreamOffset::DialogStreamOffset(QString strPassedStations, QString strDepartureTime, int NP, int hours, QWidget *parent) :
    m_route(strPassedStations), m_originalDepartureTime(strDepartureTime), m_NP(NP), m_hours(hours), ui(new Ui::DialogStreamOffset),
    QDialog(parent)
{
    ui->setupUi(this);
    ui->label_VP->setText(QString("Маршрут потока: %1").arg(m_route));
    ui->label_VP->setWordWrap(true);
    ui->label_KP->setText(QString("Оригинальное время отправления: %1").arg(m_originalDepartureTime));
    ui->label_NP->setText(QString("Номер потока: %1").arg(m_NP));
    ui->label_Message->setText(QString("Заявка будет сдвинута на %1 часов").arg(m_hours));
}

DialogStreamOffset::~DialogStreamOffset()
{
    delete ui;
}

void DialogStreamOffset::on_pushButton_YES_clicked()
{
    done(1);
}

void DialogStreamOffset::on_pushButton_NO_clicked()
{
    done(0);
}
