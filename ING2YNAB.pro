#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T15:06:04
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ING2YNAB
TEMPLATE = app
CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    csv.cpp \
    transactionstore.cpp \
    transaction.cpp \
    transactionmodel.cpp \
    filterengine.cpp \
    abstractfilter.cpp \
    scriptfilter.cpp \
    simplematchfilter.cpp \
    settings.cpp \
    transactionmodelfilter.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    csv.h \
    transactionstore.h \
    transaction.h \
    transactionmodel.h \
    filterengine.h \
    abstractfilter.h \
    scriptfilter.h \
    simplematchfilter.h \
    settings.h \
    transactionmodelfilter.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    images.qrc

win32:RC_FILE = images/ing2ynab.rc
