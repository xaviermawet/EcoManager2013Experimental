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
    Map/MapFrame.cpp \
    DBModule/DataPoint.cpp \
    DBModule/ExportModule.cpp \
    DBModule/Zone.cpp \
    DBModule/Race.cpp \
    DBModule/LapDetector.cpp \
    ExtensibleEllipseItem.cpp \
    RaceViewer.cpp \
    DBModule/ImportModule.cpp \
    Common/ColorizerProxyModel.cpp \
    Common/TreeItem.cpp \
    Common/HierarchicalProxyModel.cpp \
    Common/GroupProxyModel.cpp \
    Common/GroupingTreeModel.cpp \
    Utils/CSVParser.cpp \
    Utils/CSVParser2.cpp \
    Utils/Exception.cpp \
    CompetitionProxyModel.cpp \
    Plot/Scale.cpp \
    Plot/HorizontalScale.cpp \
    Plot/VerticalScale.cpp \
    Plot/PlotView.cpp \
    Plot/PlotCurve.cpp \
    Common/TreeLapInformationModel.cpp \
    Common/TreeNode.cpp \
    LapInformationProxyModel.cpp \
    Plot/PlotScene.cpp \
    Plot/PlotFrame.cpp \
    Common/LapInformationTreeModel.cpp \
    Common/TreeElement.cpp \
    LapDataCompartor.cpp

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
    Map/MapFrame.hpp \
    DBModule/DataPoint.hpp \
    DBModule/ExportModule.hpp \
    DBModule/Zone.hpp \
    DBModule/Race.hpp \
    DBModule/LapDetector.hpp \
    ExtensibleEllipseItem.hpp \
    RaceViewer.hpp \
    DBModule/ImportModule.hpp \
    Common/ColorizerProxyModel.hpp \
    Common/TreeItem.hpp \
    Common/HierarchicalProxyModel.hpp \
    Common/GroupProxyModel.hpp \
    Common/GroupingTreeModel.hpp \
    Utils/CSVParser.hpp \
    Utils/CSVParser2.hpp \
    Utils/Exception.hpp \
    CompetitionProxyModel.hpp \
    Plot/Scale.hpp \
    Plot/HorizontalScale.hpp \
    Plot/VerticalScale.hpp \
    Plot/PlotView.hpp \
    Plot/PlotCurve.hpp \
    Common/TreeLapInformationModel.hpp \
    Common/TreeNode.hpp \
    LapInformationProxyModel.hpp \
    Plot/PlotScene.hpp \
    Plot/PlotFrame.hpp \
    Common/LapInformationTreeModel.hpp \
    Common/TreeElement.hpp \
    LapDataCompartor.hpp

FORMS    += MainWindow.ui \
    CompetitionEntryDialog.ui \
    Map/SampleLapViewer.ui \
    Map/MapFrame.ui \
    Plot/PlotFrame.ui \
    LapDataCompartor.ui

RESOURCES += \
    Resources.qrc
