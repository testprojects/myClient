#include <QDataStream>
#include "packet.h"
#include "types.h"
#include "station.h"
#include "section.h"
#include "pvr.h"
#include "stream.h"
#include <QDebug>

QByteArray Packet::toByteArray() const
{
    return m_ba;
}

Packet::Packet(const QString &str):
    m_type(TYPE_QSTRING)
{
    QDataStream out(&m_ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << str;
    qDebug() << "message about to transmitt: " << str;
}

Packet::Packet(const Station &st):
    m_type(TYPE_STATION)
{
    QDataStream out(&m_ba, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << st.number << st.type << st.name << st.latitude << st.longitude
        << st.startNumber << st.endNumber << st.distanceTillStart << st.distanceTillEnd
        << st.pvrNumber << st.loadingCapacity23 << st.loadingCapacity24_BP << st.loadingCapacity24_GSM
        << st.loadingCapacity24_PR << st.loadingCapacity25;
    for(int i = 0; i < 60; i++) out << st.loadingPossibilities23[i];
    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_BP[i];
    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_GSM[i];
    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_PR[i];
    for(int i = 0; i < 60; i++) out << st.loadingPossibilities25[i];
    out << st.roadNumber;
}

Packet::Packet(QByteArray ba, const Type &t):
    m_type(t)
{
    m_ba = ba;
    if((m_type != TYPE_NOT_DEFINED)&&(m_type != TYPE_QSTRING)&&(m_type != TYPE_STATION) && (m_type != TYPE_SECTION) &&
            (m_type != TYPE_PVR) && (m_type != TYPE_REQUEST) && (m_type != TYPE_STREAM)) {
        qDebug() << "Wrong convert type";
        exit(1);
    }
}

Station Packet::decodeToStation()
{
    QDataStream in(&m_ba, QIODevice::ReadOnly);
    Station st;
    in >> st.number >> st.type >> st.name >> st.latitude >> st.longitude
       >> st.startNumber >> st.endNumber >> st.distanceTillStart >> st.distanceTillEnd
       >> st.pvrNumber >> st.loadingCapacity23 >> st.loadingCapacity24_BP >> st.loadingCapacity24_GSM
       >> st.loadingCapacity24_PR >> st.loadingCapacity25;
    for(int i = 0; i < 60; i++) in >> st.loadingPossibilities23[i];
    for(int i = 0; i < 60; i++) in >> st.loadingPossibilities24_BP[i];
    for(int i = 0; i < 60; i++) in >> st.loadingPossibilities24_GSM[i];
    for(int i = 0; i < 60; i++) in >> st.loadingPossibilities24_PR[i];
    for(int i = 0; i < 60; i++) in >> st.loadingPossibilities25[i];
    in >> st.roadNumber;

//    out << st.number << st.type << st.name << st.latitude << st.longitude
//        << st.startNumber << st.endNumber << st.distanceTillStart << st.distanceTillEnd
//        << st.pvrNumber << st.loadingCapacity23 << st.loadingCapacity24_BP << st.loadingCapacity24_GSM
//        << st.loadingCapacity24_PR << st.loadingCapacity25;
//    for(int i = 0; i < 60; i++) out << st.loadingPossibilities23[i];
//    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_BP[i];
//    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_GSM[i];
//    for(int i = 0; i < 60; i++) out << st.loadingPossibilities24_PR[i];
//    for(int i = 0; i < 60; i++) out << st.loadingPossibilities25[i];
//    out << st.roadNumber;
    return st;
}

QString Packet::decodeToQString()
{
    QDataStream in(&m_ba, QIODevice::ReadOnly);
    QString str;
    in >> str;
    return str;
}
