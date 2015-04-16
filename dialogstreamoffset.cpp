#include "dialogstreamoffset.h"
#include "ui_dialogstreamoffset.h"

DialogStreamOffset::DialogStreamOffset(int VP, int KP, int NP, int hours, QWidget *parent) :
    m_VP(VP), m_KP(KP), m_NP(NP), m_hours(hours), QDialog(parent),
    ui(new Ui::DialogStreamOffset)
{
    ui->setupUi(this);
    ui->label_VP->setText(QString("Вид перевозок: %1").arg(m_VP));
    ui->label_KP->setText(QString("Код получателя: %1").arg(m_KP));
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
