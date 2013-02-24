#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T19:15:10
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EcoManager2013
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    ResourceInstaller.cpp \
    CompetitionEntryDialog.cpp

HEADERS  += MainWindow.hpp \
    ResourceInstaller.hpp \
    CompetitionEntryDialog.hpp

FORMS    += MainWindow.ui \
    CompetitionEntryDialog.ui

RESOURCES += \
    Resources.qrc
