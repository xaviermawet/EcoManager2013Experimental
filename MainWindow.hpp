#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include "ResourceInstaller.hpp"

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
        ~MainWindow();

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

        // Personal slots
        void LoadCompetition(int index);

    private:

        void centerOnScreen(void);
        void createRaceView(void);
        void createToolsBar(void);
        void CreateMapZone(void);
        void DisplayDataLap(void);

    protected:

        Ui::MainWindow* ui;

        QComboBox* competitionBox;

        QSqlTableModel* sectorModel;
        QSqlTableModel* competitionNameModel;
};

#endif /* __MAINWINDOW_HPP__ */
