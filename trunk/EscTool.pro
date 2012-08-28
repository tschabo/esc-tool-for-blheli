#-------------------------------------------------
#
# Project created by QtCreator 2012-08-19T18:20:37
#
#-------------------------------------------------

include(./external/qextserialport-1.2beta2/src/qextserialport.pri)

QT       += core gui

TARGET = EscTool
TEMPLATE = app


SOURCES += main.cpp\
    cabstractserialcomm.cpp \
    cprogrammer.cpp \
    cesctoolgui.cpp \
    ccsvparser.cpp \
    cescconf.cpp \
    cconfigmodel.cpp \
    ceditordelegate.cpp

HEADERS  += \
    cabstractserialcomm.h \
    cprogrammer.h \
    cesctoolgui.h \
    ccsvparser.h \
    cescconf.h \
    cconfigmodel.h \
    ceditordelegate.h

FORMS    += \
    cesctoolgui.ui

OTHER_FILES += \
    55AA_0C.blh \
    common_0C.blh \
    A55A_0C.blh \
    5AA5_0C.blh
