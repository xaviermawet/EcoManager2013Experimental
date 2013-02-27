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
#include "DBModule/ExportModule.hpp"
#include "Common/ColorizerProxyModel.hpp"
#include "Map/SampleLapViewer.hpp"

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
        void on_actionExportTrackDataInCSV_triggered(void);

        // Personal slots
        void loadCompetition(int index);
        void removeSector(const QString& competitionName, int sectorNum);
        void addSector(QString, int, IndexedPosition, IndexedPosition);
        void updateSector(QString, int, IndexedPosition, IndexedPosition);
        void clearAllData(void);

    private:

        void centerOnScreen(void);
        void createRaceView(void);
        void createToolsBar(void);
        void createMapZone(void);
        void createPlotZone(void);
        void displayDataLap(void);
        void connectSignals(void);
        void reloadRaceView(void);
        void loadSectors(const QString& competitionName); // TODO

    protected:

        Ui::MainWindow* ui;

        QComboBox* competitionBox;
        QString currentCompetition;
        GroupingTreeModel* competitionModel;
        QList< QMap<QString, QVariant> > currentTracksDisplayed;

        // Mapping
        MapScene* mapScene;
        MapFrame* mapFrame;

        // Plot
        PlotFrame* distancePlotFrame;
        PlotFrame* timePlotFrame;

        // Models
        QSqlTableModel* sectorModel;
        QSqlTableModel* competitionNameModel;
};

#endif /* __MAINWINDOW_HPP__ */
