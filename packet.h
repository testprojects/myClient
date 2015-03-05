#ifndef PACKET_H
#define PACKET_H

#include "types.h"
#include <QObject>

struct Station;
struct Section;
struct PVR;
class Request;
class Stream;
class QXmlStreamWriter;
class QXmlStreamReader;

//данные о размере пакета и о типе хранимого объекта ДОЛЖНЫ считываться вне класса

class Packet : public QObject
{
    Q_OBJECT
public:    
    //создание
    explicit Packet(QByteArray ba, const Type &t);
    explicit Packet(const QString &str);
    explicit Packet(const Station &st);
    Packet(const Section &sec);
    Packet(const PVR &pvr);
    Packet(const Request &req);
    Packet(const Stream &stream);
    Packet(const QXmlStreamWriter &xml);
    Packet(const QXmlStreamReader &xml);
    Type type() const {return m_type;}

public:
    //разбор после приёма
    QString decodeToQString();
    Station decodeToStation();
    Section decodeToSection();
    PVR     decodeToPVR();
    Request decodeToRequest();
    Stream  decodeToStream();
    QByteArray toByteArray() const;

private:
    //исключение
    class WrongConvertType{};
    class AlreadyWritten{};

private:
    Type m_type;
    QByteArray m_ba;
};

#endif // PACKET_H
