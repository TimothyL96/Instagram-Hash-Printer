#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T13:21:41
#
#-------------------------------------------------

QT       += core gui printsupport
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = instaTagPrint
TEMPLATE = app

CONFIG += openssl-linked

RC_ICONS = icontag.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    viewimage.cpp

HEADERS  += mainwindow.h \
    viewimage.h

FORMS    += mainwindow.ui \
    viewimage.ui

DISTFILES +=

RESOURCES += \
    loadimg.qrc
