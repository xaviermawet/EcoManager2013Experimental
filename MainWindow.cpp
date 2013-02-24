#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    // GUI Configuration
    this->ui->setupUi(this);

    // Check if the database exists otherwise the database is created
    ResourceInstaller dbInstaller("EcoMotion", "EcoManager2013");
    if (!dbInstaller.CheckSettings())
        dbInstaller.InstallSettings(QDir::current(), "EcoMotion2013.db");

    // Building of the parts of the MainWindow
    this->createRaceView();
    this->createToolsBar();

    // Display Configuration
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    this->centerOnScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAboutQt_triggered(void)
{
    qApp->aboutQt();
}

void MainWindow::on_actionQuit_triggered(void)
{
    qApp->quit();
}

void MainWindow::on_actionImport_triggered(void)
{
    QMessageBox::information(this, "Action Import", "Importation des données");
}

void MainWindow::on_actionAboutEcoManager2013_triggered(void)
{
    QMessageBox::information(this, "Action About EcoManager 2013",
                             "A propos du project EcoManager 2013 ....");
}

void MainWindow::on_actionExportConfigurationModule_triggered(void)
{
    QMessageBox::information(this, "Action Export Configuration Module",
                             "Exportation de la configuration du module...");
}

void MainWindow::on_actionExportData_triggered(void)
{
    QMessageBox::information(this, "Action Export Data",
                             "Exportation de données ...");
}

/* Event that occured when the user double click on a race tree view item
 * Only lap can be double-clicked
 */
void MainWindow::on_raceView_doubleClicked(const QModelIndex& index)
{
    QModelIndex parent = index.parent();
    int depth(0);

    while (parent.isValid())
    {
        parent = parent.parent();
        depth++;
    }

    // check if a lap has been selected
    if (depth == 2)
        this->DisplayDataLap();
    else
        QMessageBox::information(this, tr("Erreur"),
                                 tr("Vous devez double-cliquer sur un tour"));
}

void MainWindow::on_actionDelimitingSectors_triggered(void)
{
    QMessageBox::information(this, "chooseSampleLap", "chooseSampleLap");
}

void MainWindow::LoadCompetition(int index)
{
    QMessageBox::information(this, "Combo competition", "index changed to "
                             + QString::number(index));
}

void MainWindow::centerOnScreen(void)
{
    QDesktopWidget screen;

    QRect screenGeom = screen.screenGeometry(this);

    int screenCenterX = screenGeom.center().x();
    int screenCenterY = screenGeom.center().y();

    this->move(screenCenterX - width () / 2, screenCenterY - height() / 2);
}

void MainWindow::createRaceView(void)
{
    // Mainly developed with Qt Designer

    this->ui->raceView->expandAll();
    this->ui->raceView->resizeColumnToContents(0);
}

void MainWindow::createToolsBar(void)
{
    // Mainly developed with Qt Designer

    this->competitionNameModel = new QSqlTableModel(this);
    this->competitionNameModel->setTable("COMPETITION");
    this->competitionNameModel->removeRows(1, 2);

    // Create a comboBox used to selecting a competition
    this->competitionBox = new QComboBox();
    this->competitionBox->setEditable(false);
    this->competitionBox->setModel(this->competitionNameModel);
    this->competitionBox->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Expanding);

    // Add the comboBox to the toolBar of the MainWindow
    this->ui->mainToolBar->insertWidget(this->ui->actionDelimitingSectors,
                                        this->competitionBox);

    QObject::connect(this->competitionBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(LoadCompetition(int)));

    this->competitionNameModel->select();
}

void MainWindow::CreateMapZone(void)
{

}

void MainWindow::DisplayDataLap(void)
{
    QMessageBox::information(this, "Affichage tour", "Double clic sur un tour");
}
