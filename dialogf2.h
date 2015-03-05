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
    int VP_Start, VP_End;
    int KP_Start, KP_End;
    int NP_Start, NP_End;
    QString grif;//security classification O_o
private slots:
    void on_buttonBox_accepted();
};

#endif // DIALOGF2_H
