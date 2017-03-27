#-------------------------------------------------
#
# Project created by QtCreator 2016-07-05T18:30:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qReplay
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    card.cpp \
    config.cpp \
    deck.cpp \
    unit.cpp \
    replay.cpp

HEADERS  += mainwindow.h \
    card.h \
    config.h \
    deck.h \
    unit.h \
    replay.h

FORMS    += mainwindow.ui

CONFIG += static


win32:RC_ICONS += internet_browse.ico
