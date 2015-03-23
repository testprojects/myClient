#ifndef DIALOGF2_H
#define DIALOGF2_H

#include <QDialog>

namespace Ui {
class DialogF2;
}

class DialogF2 : public QDialog
{
    Q_OBJECT

public:
    explicit DialogF2(QWidget *parent = 0);
    ~DialogF2();

private:
    Ui::DialogF2 *ui;

public:
    int VP;
    int KP_Start, KP_End;
    int NP_Start, NP_End;
    QString grif;//security classification O_o
    bool devideByOKR;
    bool devideByKG;
    QString actionOKR;
    QString okr;
private slots:
    void on_buttonBox_accepted();
    void on_checkBox_OKR_stateChanged(int arg1);
    void on_lineEdit_VP_Start_textChanged(const QString &arg1);
};

#endif // DIALOGF2_H
