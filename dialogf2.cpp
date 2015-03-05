#include "dialogf2.h"
#include "ui_dialogf2.h"

DialogF2::DialogF2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogF2)
{
    ui->setupUi(this);
}

DialogF2::~DialogF2()
{
    delete ui;
}

void DialogF2::on_buttonBox_accepted()
{
    VP_Start = ui->lineEdit_VP_Start->text().toInt();
    VP_End = ui->lineEdit_VP_End->text().toInt();
    KP_Start = ui->lineEdit_KP_Start->text().toInt();
    KP_End = ui->lineEdit_KP_End->text().toInt();
    NP_Start = ui->lineEdit_NP_Start->text().toInt();
    NP_End = ui->lineEdit_NP_End->text().toInt();
    grif = ui->comboBox->currentText();
}
