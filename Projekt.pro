#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T16:37:18
#
#-------------------------------------------------

QT       += core gui
QT += network
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Projekt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serverthread.cpp \
    clientthread.cpp \
    consultantthread.cpp

HEADERS  += mainwindow.h \
    serverthread.h \
    clientthread.h \
    consultantthread.h \
    config.h

FORMS    += mainwindow.ui
