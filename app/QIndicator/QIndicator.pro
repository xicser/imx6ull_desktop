#-------------------------------------------------
#
# Project created by QtCreator 2020-09-19T20:56:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QIndicator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    indicator.cpp

HEADERS  += mainwindow.h \
    indicator.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
