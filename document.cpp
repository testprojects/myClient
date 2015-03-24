#include "document.h"

#include <QtCore/QXmlStreamReader>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
//#include <QtWidgets/QMessageBox>
#include <QDebug>

Document::Document() : rootDomDocument(0)
{
}

Document::~Document()
{
    delete rootDomDocument;
}

bool Document::openTemplate()
{
    rootDomDocument = new QDomDocument;
    QFile file("ppz.xml");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "error read file template";
        return false;
    }
    rootDomDocument->setContent(&file);
    file.close();
    return true;
}

bool Document::parsingXmlForm2(QByteArray &ba)
{
    openTemplate();

    QXmlStreamReader xmlReader(ba);     //заполненный XML
    QStringList streamStr = QStringList();

    do
    {
        xmlReader.readNext();
        if (xmlReader.isStartElement()) {
            if (xmlReader.name() == "document") {
            }
            else if (xmlReader.name() == "stream") {
                if (!streamStr.isEmpty())
                    addRowToForm2(streamStr);

                streamStr.clear();
            }
            else
                streamStr << xmlReader.readElementText();
        }
    } while(!xmlReader.atEnd());

    if (xmlReader.hasError()) {
        qDebug() << "Parsing XML: " << xmlReader.errorString();
//        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка при чтении данных: %1").arg(xmlReader.errorString()));
        return false;
    }
    return true;
}

void Document::addRowToForm2(const QStringList &dataList)
{
    QDomElement tableElement = rootDomDocument->elementsByTagName("w:tbl").at(0).toElement();
    QDomElement newRowElement;
    QDomElement colElement;
    TypeMerge typeMerge = NoMerge;
    QStringList nameCarried = dataList[5].split("  ");

    for (int row = 0; row < 2; row++) {
        newRowElement = rootDomDocument->createElement("w:tr");

        for (int col = 0; col < 17; col++) {
            colElement = rootDomDocument->createElement("w:tc");

            if (row == 0)
                typeMerge = Begin;
            else
                typeMerge = Continued;

            if (row == 0 && col > 2 && col < 12)
                continue;

            switch (col) {
                case 0:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(QString("%1/%2").arg(dataList[0], dataList[1])));
                    colElement.appendChild(addTextElement(dataList[2]));
                    break;
                case 1:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[3]));
                    break;
                case 2:
                    if (row == 0) {
                        colElement.appendChild(addColumnPropertyElement(NoMerge, 10));
                        colElement.appendChild(addTextElement(QString(dataList[4].toLatin1())));
                    }
                    else
                        colElement.appendChild(addTextElement(nameCarried[0]));
                    break;
                case 3:
                    colElement.appendChild(addTextElement(nameCarried[1]));
                    break;
                case 4:
                    colElement.appendChild(addTextElement(nameCarried[2]));
                    break;
                case 5:
                    colElement.appendChild(addTextElement(nameCarried[3]));
                    break;
                case 6:
                    colElement.appendChild(addTextElement(nameCarried[4]));
                    break;
                case 7:
                    colElement.appendChild(addTextElement(nameCarried[5]));
                    break;
                case 8:
                    colElement.appendChild(addTextElement(nameCarried[6]));
                    break;
                case 9:
                    colElement.appendChild(addTextElement(nameCarried[7]));
                    break;
                case 10:
                    colElement.appendChild(addTextElement(nameCarried[8]));
                    break;
                case 11:
                    colElement.appendChild(addTextElement(nameCarried[9]));
                    break;
                case 12:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[6]));
                    colElement.appendChild(addTextElement(dataList[7]));
                    break;
                case 13:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[8]));
                    colElement.appendChild(addTextElement(dataList[9]));
                    colElement.appendChild(addTextElement(dataList[10]));
                    break;
                case 14:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[11]));
                    colElement.appendChild(addTextElement(dataList[12]));
                    colElement.appendChild(addTextElement(dataList[13]));
                    break;
                case 15:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[14]));
                    colElement.appendChild(addTextElement(dataList[15]));
                    colElement.appendChild(addTextElement(dataList[16]));
                    break;
                case 16:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    QStringList route = dataList[17].split("  ");
                    for (int i = 0; i < route.size(); i++)
                        colElement.appendChild(addTextElement(route[i]));
                    break;
            }
            newRowElement.appendChild(colElement);
        }
        tableElement.appendChild(newRowElement);
    }
}

QDomElement Document::addTextElement(const QString &text)
{
    QDomElement p = rootDomDocument->createElement("w:p");
    QDomElement r = rootDomDocument->createElement("w:r");
    QDomElement t = rootDomDocument->createElement("w:t");
    QDomText domText = rootDomDocument->createTextNode(text);

    t.appendChild(domText);
    r.appendChild(t);
    p.appendChild(r);

    return p;
}

QDomElement Document::addColumnPropertyElement(TypeMerge merge, int span)
{
    QDomElement tcPr = rootDomDocument->createElement("w:tcPr");

    if (merge) {
        QDomElement vmerge = rootDomDocument->createElement("w:vmerge");

        if (merge == Begin)
            vmerge.setAttribute("w:val", "restart");

        tcPr.appendChild(vmerge);
    }

    if (span > 0) {
        QDomElement gridSpan = rootDomDocument->createElement("w:gridSpan");
        gridSpan.setAttribute("w:val", QString::number(span));
        tcPr.appendChild(gridSpan);
    }

    return tcPr;
}

void Document::create(QByteArray &ba)
{
    if (parsingXmlForm2(ba)) {
        QFile file("ppz_out.xml");
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream(&file) << rootDomDocument->toString();
            file.close();
        }
    }
}
