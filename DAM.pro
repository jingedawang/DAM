#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T11:10:50
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DAM
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialcommunication.cpp \
    command.cpp \
    registers.cpp

HEADERS  += mainwindow.h \
    serialcommunication.h \
    command.h \
    registers.h

FORMS    += mainwindow.ui
