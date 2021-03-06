#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T11:41:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kafkacli
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
CONFIG += c++11

LIBS += -L /Users/zh/.local/lib -lrdkafka++
INCLUDEPATH +=/Users/zh/.local/include

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    connectbrokerdlg.cpp \
    partitiondetaildlg.cpp \
    kafkaconfig.cpp \
    kafkaconsumer.cpp \
    kafkaeventcb.cpp \
    kafkabasicconfig.cpp \
    messagefilterdlg.cpp \
    libfuncmgr.cpp \
    filterthread.cpp \
    filterfuncloader.cpp

HEADERS += \
        mainwindow.h \
    connectbrokerdlg.h \
    kafkaconsumer.h \
    logger.h \
    partitiondetaildlg.h \
    kafkaconfig.h \
    kafkaeventcb.h \
    kafkabasicconfig.h \
    messagefilterdlg.h \
    libfuncmgr.h \
    filterthread.h \
    filterfuncloader.h \
    filterfunc.h \
    parserfunc.h \
    parserfuncloader.h

FORMS += \
        mainwindow.ui \
    connectbrokerdlg.ui \
    partitiondetaildlg.ui \
    messagefilterdlg.ui


ICON = app.icns

RESOURCES +=

DISTFILES += \
    readme.md
