#-------------------------------------------------
#
# Project created by QtCreator 2020-10-03T21:31:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAttitudeSensor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    meter.cpp \
    icm20608.cpp

HEADERS  += mainwindow.h \
    meter.h \
    config.h \
    icm20608.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
