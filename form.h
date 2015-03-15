#ifndef FORM_H
#define FORM_H

#include <QtWidgets>
#include "client.h"

namespace Ui {
class Form;
}

class QDomDocument;
class QDomElement;
class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

private slots:
    void displayMessage(const QString &message);
    void on_pushButtonConnect_clicked();
    void onConnected();
    void onDisconnected();    

private slots:
    void on_pushButtonSendRequest_clicked();
    void on_pushButtonLoadRequestDikon_clicked();
    void on_pushButtonLoadRequestZhenya_clicked();

private slots:
    void slotPlanStarted();
    void slotStreamPlanned(int count, int amount);
    void slotStreamsFailed(int count);
    void slotPlanFinished();
    void slotOffsetStream(int VP, int KP, int NP, int hours);
    void parsingXmlForm2(QByteArray &ba);

    void on_pushButtonGetF2_clicked();

private:
    QDomDocument* openTemplate();
    void formationDocument(QDomDocument*, const QStringList&);
    QDomElement addTextElement(QDomDocument, const QString &text = QString());

    Ui::Form *ui;
    Client *m_client;

    QProgressBar *m_progressBar;
    QLabel *m_labelStreamsPlanned;
    QVBoxLayout *m_layoutProgress;
};

#endif // FORM_H
