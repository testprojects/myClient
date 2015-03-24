#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore/QObject>

class QDomDocument;
class QDomElement;
class QStringList;
class QString;
class QByteArray;
class Document : public QObject
{
        enum TypeMerge {NoMerge, Begin, Continued};
    public:
        Document();
        ~Document();

        void create(QByteArray &ba);
    private:
        bool openTemplate();
        bool parsingXmlForm2(QByteArray &ba);
        void addRowToForm2(const QStringList&);
        QDomElement addTextElement(const QString &text);
        QDomElement addColumnPropertyElement(TypeMerge merge, int span = 0);

        QDomDocument *rootDomDocument;
};

#endif // DOCUMENT_H
