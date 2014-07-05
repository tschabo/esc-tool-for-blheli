#-------------------------------------------------
#
# Project created by QtCreator 2012-08-31T19:52:49
#
#-------------------------------------------------

include($$PWD/external/qextserialport-1.2beta2/src/qextserialport.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = escdude
TEMPLATE = app


SOURCES += main.cpp\
        escdude.cpp \
    cserialcommunication.cpp \
    cprogrammer.cpp \
    cescconf.cpp \
    ceepromdefinitonfileparser.cpp \
    cconfigmodel.cpp \
    ceditordelegate.cpp

HEADERS  += escdude.h \
    common.h \
    cserialcommunication.h \
    cprogrammer.h \
    cescconf.h \
    ceepromdefinitonfileparser.h \
    cconfigmodel.h \
    ceditordelegate.h

FORMS    += escdude.ui

OTHER_FILES += \
    damped_esc.list

