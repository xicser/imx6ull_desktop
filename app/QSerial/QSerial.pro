#-------------------------------------------------
#
# Project created by QtCreator 2020-09-29T22:56:31
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSerial
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    config.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
