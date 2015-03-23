#include "dialogf2.h"
#include "ui_dialogf2.h"

DialogF2::DialogF2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogF2)
{
    ui->setupUi(this);
    ui->comboBox_Action->hide();
    ui->comboBox_OKR->hide();
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
    devideByKG = ui->checkBox_KG->isChecked();
    devideByOKR = ui->checkBox_OKR->isChecked();
    if(devideByOKR) {
        actionOKR = ui->comboBox_Action->currentText();
        okr = ui->comboBox_OKR->currentText();
    }
    else {
        actionOKR = "";
        okr = "";
    }
}

void DialogF2::on_checkBox_OKR_stateChanged(int arg1)
{
    ui->comboBox_Action->setVisible(arg1);
    ui->comboBox_OKR->setVisible(arg1);
}
