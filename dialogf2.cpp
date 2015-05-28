#include "dialogf2.h"
#include "ui_dialogf2.h"

DialogF2::DialogF2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogF2)
{
    ui->setupUi(this);

    // control input data
    ui->lineEdit_VP_Start->setValidator(new QIntValidator());
    ui->lineEdit_KP_Start->setValidator(new QIntValidator());
    ui->lineEdit_KP_End->setValidator(new QIntValidator());
    ui->lineEdit_NP_Start->setValidator(new QIntValidator());
    ui->lineEdit_NP_End->setValidator(new QIntValidator());

//    ui->comboBox_Action->hide();
//    ui->checkBox_ZVO->hide();
//    ui->checkBox_VVO->hide();
//    ui->checkBox_UVO->hide();
//    ui->checkBox_CVO->hide();
//    ui->checkBox_SF->hide();
}

DialogF2::~DialogF2()
{
    delete ui;
}

void DialogF2::on_buttonBox_accepted()
{
    VP = ui->lineEdit_VP_Start->text().toInt();
    KP_Start = ui->lineEdit_KP_Start->text().toInt();
    KP_End = ui->lineEdit_KP_End->text().toInt();
    NP_Start = ui->lineEdit_NP_Start->text().toInt();
    NP_End = ui->lineEdit_NP_End->text().toInt();
    grif = ui->comboBox->currentText();
    devideByKG = ui->checkBox_KG->isChecked();
    devideByOKR = ui->groupBox_OKR->isChecked();
    if(devideByOKR) {
        actionOKR = ui->comboBox_Action->currentIndex() + 1;    //как разделять по округам ["Прибытие в округ - 1", "Убытие из округа - 2", "Транзит через округ - 3"]
        okr = "";
        //(10-ЗВО, 20-ВВО, 30-ЮВО, 34-ЦВО)
        if(ui->checkBox_ZVO->isChecked()) okr += "10;";
        if(ui->checkBox_VVO->isChecked()) okr += "20;";
        if(ui->checkBox_UVO->isChecked()) okr += "30;";
        if(ui->checkBox_CVO->isChecked()) okr += "34;";
        if(ui->checkBox_SF->isChecked()) okr += "40;";
        if(okr.endsWith(';')) okr.chop(1);
    }
    else {
        actionOKR = 0;
        okr = "";
    }
}

//void DialogF2::on_checkBox_OKR_stateChanged(int arg1)
//{
//    ui->comboBox_Action->setVisible(arg1);
//    ui->checkBox_ZVO->setVisible(arg1);
//    ui->checkBox_VVO->setVisible(arg1);
//    ui->checkBox_UVO->setVisible(arg1);
//    ui->checkBox_CVO->setVisible(arg1);
//    ui->checkBox_SF->setVisible(arg1);
//}

void DialogF2::on_lineEdit_VP_Start_textChanged(const QString &arg1)
{
    if((arg1.toInt() >= 24) && (arg1.toInt() <= 25))
        ui->checkBox_KG->setEnabled(true);
    else
        ui->checkBox_KG->setDisabled(false);
}
