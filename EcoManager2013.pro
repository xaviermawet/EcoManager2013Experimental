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
    CompetitionEntryDialog.cpp \
    Map/SampleLapViewer.cpp \
    Map/MapView.cpp \
    Common/IndexedPosition.cpp \
    Common/CoordinateItem.cpp \
    Map/SectorItem.cpp \
    Map/CurvePathBuilder.cpp \
    Map/AnimateSectorItem.cpp \
    Map/TrackItem.cpp \
    Map/TickItem.cpp \
    Map/PathBuilder.cpp \
    Map/MapScene.cpp \
    Common/ColorPicker.cpp \
    DBModule/GeoCoordinate.cpp \
    Map/MapFrame.cpp

HEADERS  += MainWindow.hpp \
    ResourceInstaller.hpp \
    CompetitionEntryDialog.hpp \
    Map/SampleLapViewer.hpp \
    Map/MapView.hpp \
    Common/IndexedPosition.hpp \
    Common/CoordinateItem.hpp \
    Map/SectorItem.hpp \
    Map/CurvePathBuilder.hpp \
    Map/AnimateSectorItem.hpp \
    Map/TrackItem.hpp \
    Map/TickItem.hpp \
    Map/PathBuilder.hpp \
    Map/MapScene.hpp \
    Common/ColorPicker.hpp \
    DBModule/GeoCoordinate.hpp \
    Map/MapFrame.hpp

FORMS    += MainWindow.ui \
    CompetitionEntryDialog.ui \
    Map/SampleLapViewer.ui \
    Map/MapFrame.ui

RESOURCES += \
    Resources.qrc
