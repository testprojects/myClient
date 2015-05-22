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
    explicit DialogStreamOffset(QString strPassedStations, QString strDepartureTime, int NP, int hours, QWidget *parent = 0);
    ~DialogStreamOffset();

private slots:

    void on_pushButton_YES_clicked();

    void on_pushButton_NO_clicked();

private:
    QString m_route;
    QString m_originalDepartureTime;
    int m_NP;
    int m_hours;
    Ui::DialogStreamOffset *ui;
};

#endif // DIALOGSTREAMOFFSET_H
