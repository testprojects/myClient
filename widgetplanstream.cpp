#include "widgetplanstream.h"
#include "ui_widgetplanstream.h"


WidgetPlanStream::WidgetPlanStream(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::WidgetPlanStream)
{
    ui->setupUi(this);
}

WidgetPlanStream::~WidgetPlanStream()
{
    delete ui;
}

void WidgetPlanStream::on_pushButton_Send_clicked()
{
    VP = ui->lineEdit_VP->text().toInt();
    KP = ui->lineEdit_KP->text().toInt();
    NP_Start = ui->lineEdit_NP_Start->text().toInt();
    NP_End = ui->lineEdit_NP_End->text().toInt();
    switch(ui->checkBox_SUZ->checkState()) {
    case Qt::Checked: SUZ = true; break;
    case Qt::Unchecked: SUZ = false; break;
    }
    done(PLAN_STREAM_CLICKED);
}

void WidgetPlanStream::on_pushButton_Cancel_clicked()
{
    done(CANCEL_CLICKED);
}
