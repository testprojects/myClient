#-------------------------------------------------
#
# Project created by QtCreator 2015-02-14T15:12:16
#
#-------------------------------------------------

QT       += network gui widgets
INCLUDEPATH += /Users/artem/projects/plan/
TARGET = myClient
TEMPLATE = app

SOURCES += main.cpp \
    client.cpp \
    form.cpp \
    packet.cpp \
    widgetplanstream.cpp
#    ../plan/echelon.cpp \
#    ../plan/mytime.cpp \
#    ../plan/ps.cpp \
#    ../plan/pvr.cpp \
#    ../plan/request.cpp \
#    ../plan/section.cpp \
#    ../plan/station.cpp \
#    ../plan/stream.cpp

HEADERS += \
    client.h \
    form.h \
    packet.h \
    types.h \
    ../plan/echelon.h \
    ../plan/mytime.h \
    ../plan/ps.h \
    ../plan/pvr.h \
    ../plan/request.h \
    ../plan/section.h \
    ../plan/station.h \
    ../plan/stream.h \
    widgetplanstream.h

FORMS += \
    form.ui \
    widgetplanstream.ui
