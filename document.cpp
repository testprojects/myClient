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
    QStringList dataStr = QStringList();

    do
    {
        xmlReader.readNext();

        if (xmlReader.isStartElement()) {
            if (xmlReader.name() == "document") {
            }
            else if (xmlReader.name() == "district" || xmlReader.name() == "codeCargo") {
                addHeaderGroup(xmlReader.readElementText());
            }
            else if (xmlReader.name() != "stream")
                dataStr << xmlReader.readElementText();
        }
        else if (xmlReader.isEndElement())
            if (xmlReader.name() == "stream") {
                if (!dataStr.isEmpty())
                    addRowToForm2(dataStr);

                dataStr.clear();
            }
    } while(!xmlReader.atEnd());

    if (xmlReader.hasError()) {
        qDebug() << "Parsing XML: " << xmlReader.errorString();
//        QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка при чтении данных: %1").arg(xmlReader.errorString()));
        return false;
    }
    return true;
}

void Document::addHeaderGroup(const QString &header)
{
    QDomElement tableElement = rootDomDocument->elementsByTagName("w:tbl").at(0).toElement();
    QDomElement rowElement = rootDomDocument->createElement("w:tr");
    QDomElement colElement = rootDomDocument->createElement("w:tc");

    colElement.appendChild(addColumnPropertyElement(NoMerge, 16));
    colElement.appendChild(addTextElement(header, "center"));

    rowElement.appendChild(colElement);
    tableElement.appendChild(rowElement);
}

void Document::addRowToForm2(const QStringList &dataList)
{
    QDomElement tableElement = rootDomDocument->elementsByTagName("w:tbl").at(0).toElement();
    QDomElement newRowElement;
    QDomElement colElement;
    TypeMerge typeMerge = NoMerge;
    QStringList nameCarried = dataList[4].split("  ");

    for (int row = 0; row < 2; row++) {
        newRowElement = rootDomDocument->createElement("w:tr");

        for (int col = 0; col < 16; col++) {
            colElement = rootDomDocument->createElement("w:tc");

            if (row == 0)
                typeMerge = Begin;
            else
                typeMerge = Continued;

            if (row == 0 && col > 1 && col < 11)
                continue;

            switch (col) {
                case 0:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(QString("%1/%2").arg(dataList[0], dataList[1])));
                    colElement.appendChild(addTextElement(dataList[2]));        // if value == 0 => 1
                    break;
//                case 1:
//                    colElement.appendChild(addColumnPropertyElement(typeMerge));
//                    colElement.appendChild(addTextElement(dataList[3]));
//                    break;
                case 1:
                    if (row == 0) {
                        colElement.appendChild(addColumnPropertyElement(NoMerge, 10));
                        colElement.appendChild(addTextElement(QString(dataList[3].toLatin1())));
                    }
                    else
                        colElement.appendChild(addTextElement(nameCarried[0]));
                    break;
                case 2:
                    colElement.appendChild(addTextElement(nameCarried[1]));
                    break;
                case 3:
                    colElement.appendChild(addTextElement(nameCarried[2]));
                    break;
                case 4:
                    colElement.appendChild(addTextElement(nameCarried[3]));
                    break;
                case 5:
                    colElement.appendChild(addTextElement(nameCarried[4]));
                    break;
                case 6:
                    colElement.appendChild(addTextElement(nameCarried[5]));
                    break;
                case 7:
                    colElement.appendChild(addTextElement(nameCarried[6]));
                    break;
                case 8:
                    colElement.appendChild(addTextElement(nameCarried[7]));
                    break;
                case 9:
                    colElement.appendChild(addTextElement(nameCarried[8]));
                    break;
                case 10:
                    colElement.appendChild(addTextElement(nameCarried[9]));
                    break;
                case 11:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[5]));
                    colElement.appendChild(addTextElement(dataList[6]));
                    break;
                case 12:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[7]));
                    colElement.appendChild(addTextElement(dataList[8]));
                    colElement.appendChild(addTextElement(dataList[9]));
                    break;
                case 13:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[10]));
                    colElement.appendChild(addTextElement(dataList[11]));
                    colElement.appendChild(addTextElement(dataList[12]));
                    break;
                case 14:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    colElement.appendChild(addTextElement(dataList[13]));
                    colElement.appendChild(addTextElement(dataList[14]));
                    colElement.appendChild(addTextElement(dataList[15]));
                    break;
                case 15:
                    colElement.appendChild(addColumnPropertyElement(typeMerge));
                    QStringList route = dataList[16].split(QRegExp("\\s{2}"));
                    for (int i = 0; i < route.size(); i++) {
                        colElement.appendChild(addTextElement(route[i].section(QRegExp("\\s"), 0, 0)));
                        colElement.appendChild(addTextElement(route[i].section(QRegExp("\\s"), 1)));
                    }
                    break;
            }
            newRowElement.appendChild(colElement);
        }
        tableElement.appendChild(newRowElement);
    }
}

QDomElement Document::addTextElement(const QString &text, const QString &align)
{
    QDomElement p = rootDomDocument->createElement("w:p");
    QDomElement r = rootDomDocument->createElement("w:r");
    QDomElement t = rootDomDocument->createElement("w:t");
    QDomElement pPr = rootDomDocument->createElement("w:pPr");
    QDomElement jc = rootDomDocument->createElement("w:jc");
    QDomText domText = rootDomDocument->createTextNode(text);

    jc.setAttribute("w:val", align);

    t.appendChild(domText);
    r.appendChild(addCellFontPropertyElement());
    r.appendChild(t);
    pPr.appendChild(jc);
    p.appendChild(pPr);
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

QDomElement Document::addCellFontPropertyElement()
{
    QDomElement rPr = rootDomDocument->createElement("w:rPr");
    QDomElement rFonts = rootDomDocument->createElement("w:rFonts");
    QDomElement font = rootDomDocument->createElement("wx:font");

    rFonts.setAttribute("w:ascii", "Times New Roman");
    rFonts.setAttribute("w:h-ansi", "Times New Roman");
    font.setAttribute("wx:val", "Times New Roman");

    rPr.appendChild(rFonts);
    rPr.appendChild(font);

    return rPr;
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
