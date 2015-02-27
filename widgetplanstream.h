#ifndef WIDGETPLANSTREAM_H
#define WIDGETPLANSTREAM_H

#include <QDialog>
const int CANCEL_CLICKED = 0;
const int PLAN_STREAM_CLICKED = 1;

namespace Ui {
class WidgetPlanStream;
}

class WidgetPlanStream : public QDialog
{
    Q_OBJECT

public:
    explicit WidgetPlanStream(QDialog *parent = 0);
    ~WidgetPlanStream();

    int getVP() const {return VP;}
    int getKP() const {return KP;}
    int getNP_Start() const {return NP_Start;}
    int getNP_End() const {return NP_End;}
    bool getSUZ() const {return SUZ;}

private slots:
    void on_pushButton_Send_clicked();
    void on_pushButton_Cancel_clicked();

private:
    Ui::WidgetPlanStream *ui;
    int VP;
    int KP;
    int NP_Start;
    int NP_End;
    bool SUZ;
};

#endif // WIDGETPLANSTREAM_H
