/*
 * Pour les menus contextuels --> Créer les actions dans le designer et suivre
 * la démarche proposée sur le site :
 * http://www.developpez.net/forums/d719698/c-cpp/bibliotheques/qt/qtreeview-menu-contextuel/
 */

#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include "ResourceInstaller.hpp"
#include "Map/MapScene.hpp"
#include "Map/MapFrame.hpp"
#include "Plot/PlotFrame.hpp"
#include "Plot/HorizontalScale.hpp"
#include "Plot/VerticalScale.hpp"
#include "DBModule/ImportModule.hpp"
#include "CompetitionEntryDialog.hpp"
#include "CompetitionProxyModel.hpp"
#include "Common/GroupingTreeModel.hpp"
#include "Common/TreeLapInformationModel.hpp"
#include "DBModule/ExportModule.hpp"
#include "Common/ColorizerProxyModel.hpp"
#include "Map/SampleLapViewer.hpp"
#include "LapInformationProxyModel.hpp"
#include "Common/LapInformationTreeModel.hpp"
#include "LapDataCompartor.hpp"

// ------------ Remplacement des graphs par ceux de Qwt ----------------
#include "Plots/Plot.hpp"
#include "Plots/PlotCurve2.hpp"
#include "polifitgsl.hpp"
#include <qwt_plot_item.h>
// ------------ Remplacement des graphs par ceux de Qwt ----------------

#include <QtGui>
#include <QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow(void);

        virtual bool eventFilter(QObject* src, QEvent* event);

    private slots:

        // Autoconnect
        void on_actionAboutQt_triggered(void);
        void on_actionQuit_triggered(void);
        void on_actionImport_triggered(void);
        void on_actionAboutEcoManager2013_triggered(void);
        void on_actionExportConfigurationModule_triggered(void);
        void on_actionExportData_triggered(void);
        void on_raceView_doubleClicked(const QModelIndex& index);
        void on_actionDelimitingSectors_triggered(void);
        void on_raceView_customContextMenuRequested(const QPoint &pos);
        void on_actionDisplayRaceTableData_triggered(bool checked);
        void on_actionDisplayRaceTableUnder_triggered();
        void on_actionDisplayRaceTableAbove_triggered();
        void on_actionDisplayRaceTableOnRight_triggered();
        void on_actionDisplayRaceTableOnLeft_triggered();
        void on_actionDisplayRaceView_triggered(bool checked);
        void on_actionSaveCurrentLayout_triggered(void);
        void on_actionConfiguredLayout1_triggered(void);
        void on_actionConfiguredLayout2_triggered(void);
        void on_actionConfiguredLayout3_triggered(void);
        void on_actionConfiguredLayout4_triggered(void);
        void on_actionLapDataEraseTable_triggered(void);
        void on_actionLapDataTableResizeToContents_triggered(bool checked);
        void on_actionClearAllData_triggered(void);
        void on_raceTable_customContextMenuRequested(const QPoint &pos);
        void on_actionLapDataComparaison_triggered(void);
        void on_raceTable_doubleClicked(const QModelIndex &index);
        void on_menuLapDataTable_aboutToShow(void);
        void on_actionLapDataSelectAll_triggered(bool checked);
        void on_actionLapDataDrawSectors_triggered(void);
        void on_actionLapDataDisplayInAllViews_triggered(void);
        void on_menuEditRaceView_aboutToShow(void);
        void on_actionRaceViewDisplayLap_triggered(void);
        void on_actionRaceViewRemoveLap_triggered(void);

        // Personal slots
        void loadCompetition(int index);
        void removeSector(const QString& competitionName, int sectorNum);
        void addSector(QString, int, IndexedPosition, IndexedPosition);
        void updateSector(QString, int, IndexedPosition, IndexedPosition);

        void displayLapInformation(float timeValue, const QVariant& trackId);
        void displayLapInformation(float lowerTimeValue, float upperTimeValue,
                                   const QVariant &trackId);

        // ------------- Remplacement des graphs -------------------------------
        // Legend actions management
        void eraseCurve(void);
        void centerOnCurve(void);
        void changeCurveColor(void);
        void createPolynomialTrendline(void);
        void setPlotCurveVisibile(QwtPlotItem* item, bool visible);
        void showLegendContextMenu(const QwtPlotItem* item, const QPoint& pos);
        // ------------- Remplacement des graphs -------------------------------


        void on_showLineToolButton_toggled(bool checked);

        void on_showCrossToolButton_toggled(bool checked);

private:

        void centerOnScreen(void);
        void createRaceView(void);
        void createToolsBar(void);
        void createMapZone(void);
        void createPlotZone(void);
        void createMegaSquirtZone(void);
        void createRaceTable(void);
        void readSettings(const QString& settingsGroup);
        void writeSettings(const QString& settingsGroup) const;
        void displayDataLap(void);
        void connectSignals(void);
        void reloadRaceView(void);
        void loadSectors(const QString& competitionName);
        void highlightPointInAllView(const QModelIndex& index);

        // ------------ Remplacement des graphs par ceux de Qwt ----------------
        Plot* currentPlot(void) const;
        void  createPlotLegendContextMenu(void);
        void createPlotsZone(void);
        // ------------ Remplacement des graphs par ceux de Qwt ----------------

    protected:

        virtual void closeEvent(QCloseEvent* event);

        Ui::MainWindow* ui;

        QComboBox* competitionBox;
        QString currentCompetition;
        QList< QMap<QString, QVariant> > currentTracksDisplayed;

        // Mapping
        MapFrame* mapFrame;

        // Plot
        PlotFrame* distancePlotFrame;
        PlotFrame* timePlotFrame;
        PlotFrame* megaSquirtPlotFrame;

        // Models
        QSqlTableModel* sectorModel;
        QSqlTableModel* competitionNameModel;

        // Personal Models
        GroupingTreeModel* competitionModel;
        LapInformationTreeModel* raceInformationTableModel; //TreeLapInformationModel* raceInformationTableModel;

        // ------------ Remplacement des graphs par ceux de Qwt ----------------
        // Plot context Menu
        QMenu*     legendContextMenu;
        PlotCurve2* curveAssociatedToLegendItem;

        Plot* distanceSpeedPlot;
        Plot* timeSpeedPlot;
        // ------------ Remplacement des graphs par ceux de Qwt ----------------
};

#endif /* __MAINWINDOW_HPP__ */
