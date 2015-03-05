#ifndef DIALOGSTREAMOFFSET_H
#define DIALOGSTREAMOFFSET_H

#include <QDialog>

namespace Ui {
class DialogStreamOffset;
}

class DialogStreamOffset : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStreamOffset(int VP, int KP, int NP, int hours, QWidget *parent = 0);
    ~DialogStreamOffset();

private slots:

    void on_pushButton_YES_clicked();

    void on_pushButton_NO_clicked();

private:
    Ui::DialogStreamOffset *ui;
    int m_VP;
    int m_KP;
    int m_NP;
    int m_hours;
};

#endif // DIALOGSTREAMOFFSET_H
