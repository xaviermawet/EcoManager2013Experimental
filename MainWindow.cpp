#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    legendContextMenu(NULL), curveAssociatedToLegendItem(NULL),
    distanceSpeedPlot(NULL), timeSpeedPlot(NULL)
{
    // GUI Configuration
    this->ui->setupUi(this);

    // Check if the database exists otherwise the database is created
    ResourceInstaller dbInstaller("EcoMotion", "EcoManager2013");
    if (!dbInstaller.CheckSettings())
        dbInstaller.InstallSettings(QDir::current(), "EcoMotion2013.db");

    // Building of the parts of the MainWindow
    this->createRaceView();
    this->createRaceTable();
    this->createMapZone();
    this->createPlotZone();
    this->createMegaSquirtZone();
    this->createToolsBar();

    // Connect all the signals
    this->connectSignals();

    // ------------ Remplacement des graphs par ceux de Qwt ----------------
    this->createPlotLegendContextMenu();
    this->createPlotsZone();
    // ------------ Remplacement des graphs par ceux de Qwt ----------------

    // Display Configuration
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    this->readSettings("MainWindow");
    this->centerOnScreen();
}

MainWindow::~MainWindow(void)
{
    delete this->ui;

    // ------------ Remplacement des graphs par ceux de Qwt ----------------
    delete this->legendContextMenu;
    delete this->distanceSpeedPlot;
    delete this->timeSpeedPlot;
    // ------------ Remplacement des graphs par ceux de Qwt ----------------
}

/* Interception des events clavier emis sur la sectorView afin de pouvoir repercuter
 * la suppression logique d'un secteur sur la BD */
bool MainWindow::eventFilter(QObject* src, QEvent* event)
{
    if (src == this->ui->sectorView && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Delete)
        {
            // Check if at lease one sector is selected
            QModelIndexList selectedRows = this->ui->sectorView->selectionModel()->selectedRows();
            if (selectedRows.isEmpty())
            {
                QMessageBox::information(this,
                                       tr("Impossible de supprimer un secteur"),
                 tr("Vous devez sélectionner au moins un secteur à supprimer"));

                return QMainWindow::eventFilter(src, event);
            }

            QModelIndex selectedSector = selectedRows.first();

            if (selectedSector.isValid())
            {
                QSqlRecord sectorRec = sectorModel->record(selectedSector.row());
                int numSect = sectorRec.value(sectorModel->fieldIndex("num")).toInt();
                this->mapFrame->scene()->mergeSector(numSect - 1, numSect);
            }
        }
    }

    return QMainWindow::eventFilter(src, event);
}

void MainWindow::on_actionAboutQt_triggered(void)
{
    qApp->aboutQt();
}

void MainWindow::on_actionQuit_triggered(void)
{
    // Save the state of the mainWindow and its widgets
    this->writeSettings("MainWindow");

    qApp->quit();
}

// importData
void MainWindow::on_actionImport_triggered(void)
{
    // Select the directory that content race data and enter race information
    CompetitionEntryDialog dial;
    QString raceDirectoryPath = QFileDialog::getExistingDirectory(this);
    if (raceDirectoryPath.isEmpty() || dial.exec() != QDialog::Accepted)
    {
        QMessageBox::information(this, tr("Importation annulée"),
                                 tr("L'importation des données de la course"
                                    " a été <strong>annulée</strong>"));
        return;
    }

    ImportModule raceInformationImporter;

    if (dial.isNewlyCreated())
    {
        // Create new entry for the competition in the database
        raceInformationImporter.createCompetition(dial.competitionName(),
                                                  dial.wheelRadius()/ 100.0,
                                                  dial.place());

        // Update combobox taht contains the list of competition names
        this->competitionNameModel->select();
    }

    Race newRace(dial.competitionName());
    newRace.setDate(dial.date());

    raceInformationImporter.addRace(newRace, raceDirectoryPath);
    if (!raceInformationImporter.importSuceed())
    {
        QMessageBox::warning(this, tr("Erreur d'importation"),
                             raceInformationImporter.getErrorString());
    }
    else
    {
        this->reloadRaceView();
    }

}

void MainWindow::on_actionAboutEcoManager2013_triggered(void)
{
    QMessageBox::information(this, "Action About EcoManager 2013",
                             "A propos du project EcoManager 2013 ....");
}

void MainWindow::on_actionExportConfigurationModule_triggered(void)
{
    QFileDialog dirChooser(this);
    dirChooser.setFileMode(QFileDialog::DirectoryOnly);

    if (dirChooser.exec() == QFileDialog::Accepted)
    {
        QStringList dirsPaths = dirChooser.selectedFiles();
        QDir selectedDir(dirsPaths.at(0));

        QStringList choices;
        QSqlQuery query("select name from COMPETITION");
        query.exec();

        while (query.next())
            choices << query.value(0).toString();

        QString nameCompet;
        bool ok;

        do
        {
            nameCompet = QInputDialog::getItem(this, tr("Nom de la compétition"),
                                               QString(), choices, 0, false, &ok);
        }
        while (! ok && nameCompet.isEmpty());

        if (ok)
            ExportModule::buildSectorOutput(nameCompet, selectedDir);
    }
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
    Q_UNUSED(index);

    // Check if a valid row has been double clicked
    QModelIndexList rowsSelected = this->ui->raceView->selectionModel()->selectedRows();
    if (rowsSelected.count() <= 0)
    {
        QMessageBox::information(this, tr("Erreur"),
                                 tr("Vous devez double-cliquer sur un tour"));
    }
    else
    {
        this->displayDataLap();
    }
}

// chooseSampleLap
void MainWindow::on_actionDelimitingSectors_triggered(void)
{
    SampleLapViewer* dial = new SampleLapViewer(currentCompetition, this);
    dial->setModal(true);
    dial->resize(650, 400);

    // if the user choose a new sample lap
    if (dial->exec())
    {
        // Get track identifier
        QPair<int, int> refLap = dial->selectedReferencesLap();
        QSqlQuery posBoundaryQuery;
        posBoundaryQuery.prepare("select min(id), max(id) from POSITION where ref_lap_race = ? and ref_lap_num = ?");
        posBoundaryQuery.addBindValue(refLap.first);
        posBoundaryQuery.addBindValue(refLap.second);

        if (!posBoundaryQuery.exec() || ! posBoundaryQuery.next())
        {
            qWarning() << posBoundaryQuery.lastQuery() << posBoundaryQuery.lastError();
            return;
        }

        // Check if sectors alreay exists for the competition
        QSqlQuery existingSectorQuery;
        existingSectorQuery.prepare("select count(*) from SECTOR where ref_compet = ?");
        existingSectorQuery.addBindValue(currentCompetition);

        if (existingSectorQuery.exec() && existingSectorQuery.next())
        {
            int nbExistingSectors = existingSectorQuery.value(0).toInt();

            if (nbExistingSectors > 0)
            {
                // delete all existing sectors for the competition
                QSqlQuery delExistingSectorsQuery;
                delExistingSectorsQuery.prepare("delete from SECTOR where ref_compet = ?");
                delExistingSectorsQuery.addBindValue(currentCompetition);

                if (!delExistingSectorsQuery.exec())
                {
                    qWarning() << delExistingSectorsQuery.lastQuery() << delExistingSectorsQuery.lastError();
                    return;
                }
            }

            // Create new sector for the competition
            QSqlQuery sectorCreationQuery;
            sectorCreationQuery.prepare("insert into SECTOR (num, ref_compet, start_pos, end_pos) values (?, ?, ?, ?)");
            sectorCreationQuery.addBindValue(0);
            sectorCreationQuery.addBindValue(currentCompetition);
            sectorCreationQuery.addBindValue(posBoundaryQuery.value(0).toInt());
            sectorCreationQuery.addBindValue(posBoundaryQuery.value(1).toInt());

            if (!sectorCreationQuery.exec())
            {
                qWarning() << sectorCreationQuery.lastQuery() << sectorCreationQuery.lastError();
            }
            else
            {
                // Erase previous sectors from the map scene
                if (this->mapFrame->scene()->hasSectors())
                    this->mapFrame->scene()->clearSectors();

                // Display new sector
                loadSectors(currentCompetition);
            }
        }
        else
        {
            qWarning() << existingSectorQuery.lastQuery() << existingSectorQuery.lastError();
        }
    }
}

void MainWindow::on_raceView_customContextMenuRequested(const QPoint &pos)
{
    this->ui->menuEditRaceView->exec(
                this->ui->raceView->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionDisplayRaceTableData_triggered(bool checked)
{
    this->ui->raceTable->setVisible(checked);
}

void MainWindow::on_actionDisplayRaceTableUnder_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Vertical);
    this->ui->MapPlotAndRaceSplitter->insertWidget(1, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableAbove_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Vertical);
    this->ui->MapPlotAndRaceSplitter->insertWidget(0, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableOnRight_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Horizontal);
    this->ui->MapPlotAndRaceSplitter->insertWidget(1, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceTableOnLeft_triggered()
{
    this->ui->MapPlotAndRaceSplitter->setOrientation(Qt::Horizontal);
    this->ui->MapPlotAndRaceSplitter->insertWidget(0, this->ui->raceTable);
}

void MainWindow::on_actionDisplayRaceView_triggered(bool checked)
{
    this->ui->raceFrame->setVisible(checked);
}

void MainWindow::on_actionSaveCurrentLayout_triggered(void)
{
    bool ok(false);

    QStringList listSavedLayouts;
    listSavedLayouts << this->ui->actionConfiguredLayout1->text()
                     << this->ui->actionConfiguredLayout2->text()
                     << this->ui->actionConfiguredLayout3->text()
                     << this->ui->actionConfiguredLayout4->text();

    QString layoutSelected = QInputDialog::getItem(
                this, tr("Sauvegarde de la disposition courante"),
                tr("Choisissez l'emplacement dans lequel sauver la disposition "
                   "courante"), listSavedLayouts, 0, false, &ok);

    if (!ok)
    {
        QMessageBox::warning(this, tr("Action annulée"),
                             tr("La sauvegarde de la disposition courante à été annulée"));
        return;
    }

    // Sauvegarde des paramètres d'affichage
    this->writeSettings(layoutSelected);
    QMessageBox::information(this, tr("Sauvegarde de la disposition courante"),
                             tr("La disposition courante a correctement été "
                                "sauvée dans ") + layoutSelected);
}

void MainWindow::on_actionConfiguredLayout1_triggered(void)
{
    this->readSettings(this->ui->actionConfiguredLayout1->text());
}

void MainWindow::on_actionConfiguredLayout2_triggered(void)
{
    this->readSettings(this->ui->actionConfiguredLayout2->text());
}

void MainWindow::on_actionConfiguredLayout3_triggered(void)
{
    this->readSettings(this->ui->actionConfiguredLayout3->text());
}

void MainWindow::on_actionConfiguredLayout4_triggered(void)
{
    this->readSettings(this->ui->actionConfiguredLayout4->text());
}

void MainWindow::on_actionLapDataEraseTable_triggered(void)
{
    // Erase all highlited point or sector on the mapping view
    this->mapFrame->scene()->clearSceneSelection();

    // Erase all highlited point on the mapping view
    this->distancePlotFrame->scene()->clearPlotSelection();
    this->timePlotFrame->scene()->clearPlotSelection();

    // Remove laps information from the table
    this->raceInformationTableModel->removeRows(
                0, this->raceInformationTableModel->rowCount());
}

void MainWindow::on_actionLapDataTableResizeToContents_triggered(bool checked)
{
    if (checked)
        this->ui->raceTable->header()->setResizeMode(QHeaderView::ResizeToContents);
    else
        this->ui->raceTable->header()->setResizeMode(QHeaderView::Interactive);
}

void MainWindow::on_actionClearAllData_triggered(void)
{
    qDebug() << "On efface tout";

    // Clear the tracks of the mapping view
    this->mapFrame->scene()->clearTracks();

    // clear the curves of the graphic views
    this->distancePlotFrame->scene()->clearCurves();
    this->timePlotFrame->scene()->clearCurves();

    // Clear the list of all tracks currently displayed
    this->currentTracksDisplayed.clear();

    // Remove laps information from the table
    this->on_actionLapDataEraseTable_triggered();
}

void MainWindow::on_raceTable_customContextMenuRequested(const QPoint &pos)
{
    this->ui->menuLapDataTable->exec(
                this->ui->raceTable->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionLapDataComparaison_triggered(void)
{
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows(0);

    if (rowsSelectedIndexes.count() != 2)
        return;

    int lapNum, raceNum;
    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num for the first selected item
    LapNumModelIndex  = rowsSelectedIndexes.at(0).parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    lapNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();
    raceNum = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    QVector<QVariant> row1 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(0));
    QVector<QVariant> row2 = this->raceInformationTableModel->rowData(
                rowsSelectedIndexes.at(1));

    LapDataCompartor ldc(raceNum, lapNum, this);
    ldc.addLapsData(row1.toList(), row2.toList());
    ldc.exec();
}

void MainWindow::on_raceTable_doubleClicked(const QModelIndex &index)
{
    // Check if a valid row has been double clicked
    QModelIndexList rowsSelected = this->ui->raceTable->selectionModel()->selectedRows();
    if (rowsSelected.count() <= 0)
        return;

    this->highlightPointInAllView(index);
}

void MainWindow::on_menuLapDataTable_aboutToShow(void)
{
    // Récupérer les index de tous les éléments sélectionnés
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows();

    bool multipleRowsSelected = rowsSelectedIndexes.count() > 0;
    bool comparaisonVisible   = rowsSelectedIndexes.count() == 2 &&
                                rowsSelectedIndexes.at(0).parent() ==
                                rowsSelectedIndexes.at(1).parent();

    // Afficher dans toutes les vues
    this->ui->actionLapDataDisplayInAllViews->setVisible(multipleRowsSelected);

    // Retirer les données sélectionnées (du tableau et des vues)
    this->ui->actionLapDataRemoveFromAllViews->setVisible(multipleRowsSelected);

    /* On ne porpose de faire une comparaison entre deux données du tableau
     * si et seulement si ce sont des données issues d'une meme course et
     * du meme tour */
    this->ui->actionLapDataComparaison->setVisible(comparaisonVisible);
    this->ui->actionLapDataDrawSectors->setVisible(comparaisonVisible);
}

void MainWindow::on_actionLapDataSelectAll_triggered(bool checked)
{
    if (checked)
        this->ui->raceTable->selectAll();
    else
        this->ui->raceTable->clearSelection();
}

void MainWindow::on_actionLapDataDrawSectors_triggered(void)
{
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows(0);

    if (rowsSelectedIndexes.count() != 2)
        return;

    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num for the first selected item
    LapNumModelIndex  = rowsSelectedIndexes.at(0).parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    QMap<QString, QVariant> trackId;
    trackId["race"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    trackId["lap"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();

    // Get the time in milliseconds for the selected item
    QModelIndex rowIndex = rowsSelectedIndexes.at(0);
    float time1 = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    rowIndex.row(), 2, rowIndex.parent())).toFloat();

    rowIndex = rowsSelectedIndexes.at(1);
    float time2 = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    rowIndex.row(), 2, rowIndex.parent())).toFloat();


    //this->mapFrame->scene()->clearSceneSelection();
    this->mapFrame->scene()->highlightSector(time1, time2, trackId);

    //this->distancePlotFrame->scene()->clearPlotSelection();
    this->distancePlotFrame->scene()->highlightSector(time1, time2, trackId); // FIXME : méthode qui utilise un arondissement car prévu pour des données de temps issues de la vue mapping

    //this->timePlotFrame->scene()->clearPlotSelection();
    this->timePlotFrame->scene()->highlightSector(time1, time2, trackId); // FIXME : méthode qui utilise un arondissement car prévu pour des données de temps issues de la vue mapping
}

void MainWindow::on_actionLapDataDisplayInAllViews_triggered(void)
{
    // Récupérer les index de tous les éléments sélectionnés
    QModelIndexList rowsSelectedIndexes =
            this->ui->raceTable->selectionModel()->selectedRows();

    // Mise en évidence de tous les points sélectionnés dans toutes les vues
    foreach (QModelIndex index, rowsSelectedIndexes)
        this->highlightPointInAllView(index);
}

void MainWindow::on_menuEditRaceView_aboutToShow(void)
{
    // Si au moins un tour est sélectionné
    if (this->ui->raceView->selectionModel())
    {
        QModelIndex curIndex =
                this->ui->raceView->selectionModel()->currentIndex();

        // Si on a bien sélectionné un tour
        if (curIndex.parent().parent().isValid())
        {
            // Create trak identifier
            int ref_race = competitionModel->data(
                        competitionModel->index(curIndex.row(), 1,
                                                curIndex.parent())).toInt();
            int ref_lap = competitionModel->data(
                        competitionModel->index(curIndex.row(), 2,
                                                curIndex.parent())).toInt();

            QMap<QString, QVariant> trackIdentifier;
            trackIdentifier["race"] = ref_race;
            trackIdentifier["lap"] = ref_lap;

            bool lapAlreadyDisplayed = this->currentTracksDisplayed.contains(
                        trackIdentifier);

            this->ui->actionRaceViewDisplayLap->setVisible(!lapAlreadyDisplayed);
            this->ui->actionRaceViewRemoveLap->setVisible(lapAlreadyDisplayed);
            this->ui->actionRaceViewExportLapDataInCSV->setVisible(true);
            return;
        }
    }

    this->ui->actionRaceViewDisplayLap->setVisible(false);
    this->ui->actionRaceViewRemoveLap->setVisible(false);
    this->ui->actionRaceViewExportLapDataInCSV->setVisible(false);
}

void MainWindow::on_actionRaceViewDisplayLap_triggered(void)
{
    this->displayDataLap();
}

void MainWindow::on_actionRaceViewRemoveLap_triggered(void)
{
    // Vérifier si l'élément sur lequel le clique est effectué est bien un tour
    QModelIndex curIndex = this->ui->raceView->selectionModel()->currentIndex();

    // Create trak identifier
    int ref_race = competitionModel->data(
                competitionModel->index(curIndex.row(), 1,
                                        curIndex.parent())).toInt();
    int ref_lap = competitionModel->data(
                competitionModel->index(curIndex.row(), 2,
                                        curIndex.parent())).toInt();

    QMap<QString, QVariant> trackIdentifier;
    trackIdentifier["race"] = ref_race;
    trackIdentifier["lap"] = ref_lap;

    // Remove the lap from all the view
    this->currentTracksDisplayed.removeOne(trackIdentifier);
    if (this->mapFrame->scene()->removeTrack(trackIdentifier))
        qDebug() << "mapping Supprimé !!!";
    if (this->distancePlotFrame->scene()->removeCurves(trackIdentifier))
        qDebug() << "distance supprimé !!!";
    if (this->timePlotFrame->scene()->removeCurves(trackIdentifier))
        qDebug() << "time supprimé !!!";
}

void MainWindow::loadCompetition(int index)
{
    this->currentCompetition = competitionNameModel->record(index).value(0).toString();

    this->on_actionClearAllData_triggered(); // Clear all tracks information of each view
    this->mapFrame->scene()->clearSectors(); // clear sectors

    // clear the sector view
    if (this->sectorModel)
    {
        this->sectorModel->clear();
        this->ui->sectorView->update();
    }

    // Load races information
    this->reloadRaceView();
}

void MainWindow::removeSector(const QString &competitionName, int sectorNum)
{
    QSqlDatabase::database().driver()->beginTransaction();

    QSqlQuery delQuery("delete from sector where ref_compet = ? and num = ?");
    delQuery.addBindValue(competitionName);
    delQuery.addBindValue(sectorNum);

    if (delQuery.exec())
    {
        qDebug() << "sector deleted.";

        QSqlQuery updateNumSect("update SECTOR set num = num - 1 where ref_compet = ? and num > ?");
        updateNumSect.addBindValue(competitionName);
        updateNumSect.addBindValue(sectorNum);

        if (!updateNumSect.exec())
        {
            qWarning() << updateNumSect.lastError();
            QSqlDatabase::database().driver()->rollbackTransaction();
        }
        else
        {
            QSqlDatabase::database().driver()->commitTransaction();
            qDebug() << "sector effectively deleted.";
        }
    }
    else
    {
        qWarning() << "Unable to delete sector" << delQuery.lastError();
        QSqlDatabase::database().driver()->rollbackTransaction();
    }

    this->sectorModel->select();
    this->ui->sectorView->setVisible(this->sectorModel->rowCount() > 0);
}

void MainWindow::addSector(QString competName, int sectNum,
                           IndexedPosition firstCoord, IndexedPosition lastCoord)
{
    QSqlDatabase::database().driver()->beginTransaction();

    QSqlQuery updateNumSect("update SECTOR set num = num + 1 where ref_compet = ? and num >= ?");
    updateNumSect.addBindValue(competName);
    updateNumSect.addBindValue(sectNum);

    if (!updateNumSect.exec())
    {
        QSqlDatabase::database().driver()->rollbackTransaction();
    }
    else
    {
        QSqlQuery insertQuery("insert into SECTOR (num, ref_compet, start_pos, end_pos) values (?, ?, ?, ?)");
        insertQuery.addBindValue(sectNum);
        insertQuery.addBindValue(competName);
        insertQuery.addBindValue(firstCoord.index());
        insertQuery.addBindValue(lastCoord.index());

        if (insertQuery.exec())
        {
            QSqlDatabase::database().driver()->commitTransaction();
        }
        else
        {
            qWarning() << "Unable to insert new sector after : " << insertQuery.lastError();
            QSqlDatabase::database().driver()->rollbackTransaction();
        }
    }

    this->sectorModel->select();
    this->ui->sectorView->setVisible(true);
}

void MainWindow::updateSector(QString competName, int sectNum,
                          IndexedPosition firstCoord, IndexedPosition lastCoord)
{
    QSqlQuery updateBoundaries("update SECTOR set start_pos = ?, end_pos = ? where ref_compet = ? and num = ?");
    updateBoundaries.addBindValue(firstCoord.index());
    updateBoundaries.addBindValue(lastCoord.index());
    updateBoundaries.addBindValue(competName);
    updateBoundaries.addBindValue(sectNum);

    if (!updateBoundaries.exec())
        qWarning() << updateBoundaries.lastQuery() << updateBoundaries.lastError();
}

void MainWindow::displayLapInformation(float timeValue, const QVariant &trackId)
{
    // Get the race number and the lap number from the trackId
    QMap<QString, QVariant> trackIdentifier =
            qvariant_cast< QMap<QString, QVariant> >(trackId);
    int ref_race = trackIdentifier["race"].toInt();
    int ref_lap  = trackIdentifier["lap"].toInt();

    /* tiemeValue passé en paramètre est exprimé en secondes mais les timestamp
     * sauvées dans la base de données sont en millisecondes */
    int timestamp = timeValue * 1000;
    qDebug() << "timeValue = " << timeValue;
    qDebug() << "timestamp = " << timestamp;

    // Récupérer les informations de temps et de vitesses
    QSqlQuery query;
    query.prepare("select timestamp, value from SPEED where timestamp <= ? and ref_lap_race = ? and ref_lap_num = ? order by timestamp");
    query.addBindValue(timestamp);
    query.addBindValue(ref_race);
    query.addBindValue(ref_lap);

    if (!query.exec())
    {
        QString errorMsg("Impossible de récupérer les données numériques "
                         "associées à votre sélection pour le tour " +
                         QString::number(ref_lap) + " de la course " +
                         QString::number(ref_race));
        QMessageBox::warning(this, tr("Erreur de récupération de données"),
                             tr(errorMsg.toStdString().c_str()));
        return;
    }

    double time(0),  lastTime(0);
    double speed(0), lastSpeed(0);
    double pos(1.5),   lastPos(0); // FIXME : remplacer par la valeur entrée pour le périmètre

    // Boucle de calcul de la distance parcourue jusqu'au temps timeValue
    while(query.next())
    {
        lastTime = time;
        lastSpeed = speed;
        lastPos = pos;

        time  = query.value(0).toFloat() / 1000; // Le temps est sauvé en millisecondes dans la db et on le veut en secondes
        speed = query.value(1).toDouble();

        int multipleWheelPerimeter = ceil(((speed + lastSpeed) / (2 * 3.6)) * (time - lastTime)) / 1.5;
        pos = lastPos + multipleWheelPerimeter * 1.5;

        //qDebug() << "multipleWheelPerimeter = " << multipleWheelPerimeter;
    }

    // Calcul de l'accélération
    qreal diff = (speed -lastSpeed) / 3.6; // vitesse en m/s
    qreal acc = diff / (time - lastTime);

    QList<QVariant> lapData;
    lapData.append(QVariant());
    lapData.append(timestamp); // Tps (ms)
    lapData.append(time);      // Tps (s)
    lapData.append(pos);       // Dist (m)
    lapData.append(speed);     // V (km\h)
    lapData.append(qAbs(acc) > 2 ? "NS" : QString::number(acc)); // Acc (m\s²)
    lapData.append("RPM");     // RPM
    lapData.append("PW");      // PW

    this->raceInformationTableModel->addRaceInformation(ref_race, ref_lap, lapData);
    this->ui->raceTable->expandAll();
}

void MainWindow::displayLapInformation(float lowerTimeValue,
                                       float upperTimeValue,
                                       const QVariant &trackId)
{
    // Remove laps information from the table
    //this->raceInformationTableModel->removeRows(0, this->raceInformationTableModel->rowCount());

    // Get the race number and the lap number from the trackId
    QMap<QString, QVariant> trackIdentifier =
            qvariant_cast< QMap<QString, QVariant> >(trackId);
    int ref_race = trackIdentifier["race"].toInt();
    int ref_lap  = trackIdentifier["lap"].toInt();

    qDebug() << "Afficher les données pour  Race = " << ref_race << " LAP = " << ref_lap;

    /* upperTimeValue passé en paramètre est exprimé en secondes mais les
     * timestamp sauvées dans la base de données sont en millisecondes */
    int upperTimeStamp = upperTimeValue * 1000;

    // Récupérer les informations de temps et de vitesses
    QSqlQuery query;
    query.prepare("select timestamp, value from SPEED where timestamp <= ? and ref_lap_race = ? and ref_lap_num = ? order by timestamp");
    query.addBindValue(upperTimeStamp);
    query.addBindValue(ref_race);
    query.addBindValue(ref_lap);

    if (!query.exec())
    {
        QString errorMsg("Impossible de récupérer les données numériques "
                         "associées à votre sélection pour le tour " +
                         QString::number(ref_lap) + " de la course " +
                         QString::number(ref_race));
        QMessageBox::warning(this, tr("Erreur de récupération de données"),
                             tr(errorMsg.toStdString().c_str()));
        return;
    }

    double time(0),  lastTime(0);
    double speed(0), lastSpeed(0);
    double pos(1.5),   lastPos(0); // FIXME : remplacer par la valeur entrée pour le périmètre
    QList< QList<QVariant> > lapDataList;

    while(query.next())
    {
        lastTime = time;
        lastSpeed = speed;
        lastPos = pos;

        time  = query.value(0).toFloat() / 1000; // Le temps est sauvé en millisecondes dans la db et on le veut en secondes
        speed = query.value(1).toDouble();

        int multipleWheelPerimeter = ceil(((speed + lastSpeed) / (2 * 3.6)) * (time - lastTime)) / 1.5;
        pos = lastPos + multipleWheelPerimeter * 1.5;

        //qDebug() << "multipleWheelPerimeter = " << multipleWheelPerimeter;

        // Données a afficher dans le tableau
        if(time >= lowerTimeValue)
        {
            // Calcul de l'accélération FIXME : ne pas oublier de vérifier si c'est pas le premier point, dans ce cas, le calcul de l'accélération est impossible !!!!
            qreal diff = (speed -lastSpeed) / 3.6; // vitesse en m/s
            qreal acc  = diff / (time - lastTime);

            QList<QVariant> lapData;
            lapData.append(QVariant());
            lapData.append(time * 1000); // Tps (ms)
            lapData.append(time);        // Tps (s)
            lapData.append(pos);         // Dist (m)
            lapData.append(speed);       // V (km\h)
            lapData.append(qAbs(acc) > 2 ? "NS" : QString::number(acc)); // Acc (m\s²)
            lapData.append("RPM");       // RPM
            lapData.append("PW");        // PW

            // Ajout de la ligne de données à la liste
            lapDataList.append(lapData);
        }
    }

    // Ajout des données dans le tableau
    this->raceInformationTableModel->addMultipleRaceInformation(
                ref_race, ref_lap, lapDataList);

    this->ui->raceTable->expandAll();
}

void MainWindow::eraseCurve(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    // Delete the curve associated to the legend item
    this->curveAssociatedToLegendItem->detach();
    delete this->curveAssociatedToLegendItem;
    this->curveAssociatedToLegendItem = NULL;

    // update the plot
    this->currentPlot()->replot();
}

void MainWindow::centerOnCurve(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    this->currentPlot()->zoom(this->curveAssociatedToLegendItem);
}

void MainWindow::changeCurveColor(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    // Select a new color
    QColor newColor = QColorDialog::getColor(
                this->curveAssociatedToLegendItem->pen().color(), this,
                tr("Choisir une nouvelle couleur pour la courbe"));

    // If the user cancels the dialog, an invalid color is returned
    if (newColor.isValid())
        this->curveAssociatedToLegendItem->setPen(QPen(newColor));
}

void MainWindow::createPolynomialTrendline(void)
{
    // if no curve associated to the legend item. This shouldn't happen!
    if (this->curveAssociatedToLegendItem == NULL)
        return;

    bool ok(false);
    int degree = QInputDialog::getInt(
                this, tr("Courbe de tendance polynomiale"),
                tr("Ordre de complexité ?"), 2, 2, 100, 1, &ok);

    if (!ok) // User canceled
        return;

    // Récupération de la série des points de la courbe
    QwtPointSeriesData* curveSeriesData =
            (QwtPointSeriesData*)this->curveAssociatedToLegendItem->data();
    if (!curveSeriesData)
        return;

    // Récupération de la liste des points de la courbe
    QVector<QPointF> curvePoints(curveSeriesData->samples());

    // Calcul de tous les coefficients de l'équation
    QVector<double> coefficients(polynomialfit(curvePoints, degree));

    // Création de la liste des points de la courbe de tendance
    for(int i(0); i < curvePoints.count(); ++i)
    {
        // Calcul de la nouvelle valeur de y
        double y(0);
        for(int j(0); j < coefficients.count(); ++j)
            y += coefficients.at(j) * qPow(curvePoints.at(i).x(), j);

        curvePoints[i].setY(y);
    }

    // Création de la courbe
    QwtPointSeriesData* trendlineSeriesData = new QwtPointSeriesData(curvePoints);
    PlotCurve2* trendlineCurve = new PlotCurve2(
                this->curveAssociatedToLegendItem->title().text() +
                tr(" Poly(") + QString::number(degree) + ")",
                this->curveAssociatedToLegendItem->pen());
    trendlineCurve->setAxes(this->curveAssociatedToLegendItem->xAxis(),
                            this->curveAssociatedToLegendItem->yAxis());
    trendlineCurve->setData(trendlineSeriesData);
    trendlineCurve->attach(this->curveAssociatedToLegendItem->plot());
    this->setPlotCurveVisibile(trendlineCurve, true);
}

void MainWindow::setPlotCurveVisibile(QwtPlotItem* item, bool visible)
{
    item->setVisible(visible);
    QWidget* w = item->plot()->legend()->find(item);
    if ( w && w->inherits("QwtLegendItem") )
        ((QwtLegendItem *)w)->setChecked(visible);

    item->plot()->replot();
}

void MainWindow::showLegendContextMenu(const QwtPlotItem* item,
                                       const QPoint& pos)
{
    // Save the plot curve associated to the legend item
    this->curveAssociatedToLegendItem = (PlotCurve2*) item;
    if(!this->curveAssociatedToLegendItem)
        return;

    // Display custom contextual menu
    this->legendContextMenu->exec(pos);
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
                                        QSizePolicy::Maximum);

    // Add the comboBox to the toolBar of the MainWindow
    this->ui->mainToolBar->insertWidget(this->ui->actionDelimitingSectors,
                                        this->competitionBox);

    QObject::connect(this->competitionBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(loadCompetition(int)));

    this->competitionNameModel->select();
}

void MainWindow::createMapZone(void)
{
    // Create the map frame (the map view is included)
    this->mapFrame = new MapFrame(this);
    this->mapFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    // Manage the sector view
    this->ui->sectorView->setVisible(false);
    this->ui->sectorView->installEventFilter(this);

    // Manage the map splitter
    this->ui->mapSplitter->insertWidget(0, this->mapFrame);
    this->ui->mapSplitter->setStretchFactor(0, 3);

    // Manage sector model
    this->sectorModel = NULL;
}

void MainWindow::createPlotZone(void)
{
    // Create distance plot frame
    this->distancePlotFrame = new PlotFrame;
    HorizontalScale* distAxis = new HorizontalScale(Scale::Bottom);
    distAxis->setResolution(5);
    distAxis->setUnitLabel("d(m)");
    this->distancePlotFrame->addHorizontalAxis(distAxis);

    VerticalScale* dspeedAxis = new VerticalScale(Scale::Left);
    dspeedAxis->setResolution(1);
    dspeedAxis->setUnitLabel("v(km/h)");
    this->distancePlotFrame->addVerticalAxis(dspeedAxis);

    VerticalScale* daccAxis = new VerticalScale(Scale::Right);
    daccAxis->setResolution(1);
    daccAxis->translate(-50);
    daccAxis->scale(0.2);
    daccAxis->setUnitLabel("a(m/s²)");
    this->distancePlotFrame->addVerticalAxis(daccAxis);

    // Add the distance plot frame the the tab widget
    QVBoxLayout* distancePlotLayout = new QVBoxLayout(this->ui->tabDistance);
    distancePlotLayout->setMargin(0);
    distancePlotLayout->addWidget(this->distancePlotFrame);

    // Create time plot frame
    this->timePlotFrame = new PlotFrame;
    HorizontalScale* timeAxis = new HorizontalScale(Scale::Bottom);
    timeAxis->setResolution(5);
    timeAxis->setUnitLabel("t(s)");
    this->timePlotFrame->addHorizontalAxis(timeAxis);

    VerticalScale* tspeedAxis = new VerticalScale(Scale::Left);
    tspeedAxis->setResolution(1);
    tspeedAxis->setUnitLabel("v(km/h)");
    this->timePlotFrame->addVerticalAxis(tspeedAxis);

    VerticalScale* taccAxis = new VerticalScale(Scale::Right);
    taccAxis->setResolution(1);
    taccAxis->translate(-50);
    taccAxis->scale(0.2);
    taccAxis->setUnitLabel("a(m/s²)");
    this->timePlotFrame->addVerticalAxis(taccAxis);

    // Add the time plot frame the the tab widget
    QVBoxLayout* timePlotLayout = new QVBoxLayout(this->ui->tabTime);
    timePlotLayout->setMargin(0);
    timePlotLayout->addWidget(this->timePlotFrame);
}

void MainWindow::createMegaSquirtZone(void)
{
    // Create MegaSquirt plot frame
    this->megaSquirtPlotFrame = new PlotFrame;
    HorizontalScale* timeAxis = new HorizontalScale(Scale::Bottom);
    timeAxis->setResolution(5);
    timeAxis->setUnitLabel("t(s)");
    this->megaSquirtPlotFrame->addHorizontalAxis(timeAxis);

    // Add the plot frame to the megaSquirt splitter
    this->ui->megaSquirtSplitter->addWidget(this->megaSquirtPlotFrame);

    // Build model (model) for the comboBoxes (view/controler)
    QStringList megaSquirtParamList;
    megaSquirtParamList << "RPM" << "Batt V" << "PW" << "Gammae" << "Gair";
    QStringListModel* megaSquirtParamModel = new QStringListModel(megaSquirtParamList);

    // Apply the model to each comboBox
    this->ui->megaSquirtComboBox1->setModel(megaSquirtParamModel);
    this->ui->megaSquirtComboBox2->setModel(megaSquirtParamModel);
    this->ui->megaSquirtComboBox3->setModel(megaSquirtParamModel);
    this->ui->megaSquirtComboBox4->setModel(megaSquirtParamModel);
}

void MainWindow::createRaceTable(void)
{
    // Create the model for the table of laps information
    QStringList headers;
    headers << tr("Course") << tr("Tps(ms)") << tr("Tps(s)") << tr("Dist(m)")
            << tr("v(km\\h)") << tr("Acc (m\\s2)") << tr("RPM") << tr("PW");
    this->raceInformationTableModel = new LapInformationTreeModel(headers); //this->raceInformationTableModel = new TreeLapInformationModel(headers);

    /* Use a proxy model to manage background color of each row and manage
     * how the data for Race and lap number are displayed */
    LapInformationProxyModel* wrapper = new LapInformationProxyModel(this);
    wrapper->setSourceModel(this->raceInformationTableModel);

    // Apply the model to the table and change the selection mode
    //this->ui->raceTable->setModel(this->raceInformationTableModel);
    this->ui->raceTable->setModel(wrapper);
    this->ui->raceTable->setSelectionMode(QAbstractItemView::MultiSelection);
    this->ui->raceTable->setAlternatingRowColors(true); // Can be done with stylesheet, proxyModel or the mainModel
}

void MainWindow::readSettings(const QString& settingsGroup)
{
    QSettings settings;

    settings.beginGroup(settingsGroup);

    /* Contourne le bug non résolu par Qt de la restauration de la géométrie
     * d'une fenetre maximisée alors qu'elle est déjà maximisée */
    if (settings.value("isMaximized").toBool())
        this->showMaximized();
    else
        this->restoreGeometry(settings.value("geometry").toByteArray());

    this->ui->mainSplitter->restoreState(
                settings.value("mainSplitter").toByteArray());
    this->ui->MapPlotAndRaceSplitter->restoreState(
                settings.value("MapPlotAndRaceSplitter").toByteArray());
    this->ui->MapPlotSplitter->restoreState(
                settings.value("MapPlotSplitter").toByteArray());

    settings.endGroup();
}

void MainWindow::writeSettings(const QString& settingsGroup) const
{
    QSettings settings;

    settings.beginGroup(settingsGroup);
    settings.setValue("isMaximized", this->isMaximized());
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("mainSplitter", this->ui->mainSplitter->saveState());
    settings.setValue("MapPlotAndRaceSplitter", this->ui->MapPlotAndRaceSplitter->saveState());
    settings.setValue("MapPlotSplitter", this->ui->MapPlotSplitter->saveState());
    settings.endGroup();
}

void MainWindow::displayDataLap(void)
{
    QModelIndex curIndex = this->ui->raceView->selectionModel()->currentIndex();

    if (curIndex.parent().parent().isValid())
    {
        // Create trak identifier
        int ref_race = competitionModel->data(competitionModel->index(curIndex.row(), 1, curIndex.parent())).toInt();
        int ref_lap = competitionModel->data(competitionModel->index(curIndex.row(), 2, curIndex.parent())).toInt();

        QMap<QString, QVariant> trackIdentifier;
        trackIdentifier["race"] = ref_race;
        trackIdentifier["lap"] = ref_lap;

        // Check if the track is already displayed
        if (this->currentTracksDisplayed.contains(trackIdentifier))
        {
            QMessageBox::warning(this, tr("Opération impossible"),
                                 tr("Ce tour est déjà affiché dans les différentes vues"));
            return;
        }
        else
        {
            qDebug("tour pas encore affiché");
            this->currentTracksDisplayed.append(trackIdentifier);
        }

        /* ------------------------------------------------------------------ *
         *                         Populate map scene                         *
         * ------------------------------------------------------------------ */
        QSqlQuery posQuery("select longitude, latitude, timestamp from POSITION where ref_lap_race = ? and ref_lap_num = ? order by timestamp");
        posQuery.addBindValue(ref_race);
        posQuery.addBindValue(ref_lap);

        if (posQuery.exec())
        {
            QVector<QPointF> pos;
            QVector<float> indexValues;

            while (posQuery.next())
            {
                GeoCoordinate tmp;
                tmp.setLongitude(posQuery.value(0).toFloat());
                tmp.setLatitude(posQuery.value(1).toFloat());

                //see projection method doc for usage purpose
                pos.append(tmp.projection());
                indexValues << posQuery.value(2).toFloat() / 1000;
            }

            qDebug() << posQuery.lastQuery();
            qDebug() <<  ref_race << curIndex.row() << pos.size();
            this->mapFrame->scene()->addTrack(pos, indexValues, trackIdentifier);
        }

        // If a sampling lap has already be defined, just load it in the view
        if (!this->mapFrame->scene()->hasSectors())
            this->loadSectors(this->currentCompetition);

        /* ------------------------------------------------------------------ *
         *         Populate plot frames (play the role of plot scene )        *
         * ------------------------------------------------------------------ */
        QSqlQuery speedQuery;
        speedQuery.prepare("select timestamp, value from SPEED where ref_lap_race = ? and ref_lap_num = ? order by timestamp");
        speedQuery.addBindValue(ref_race);
        speedQuery.addBindValue(ref_lap);

        if (speedQuery.exec())
        {
            QList<IndexedPosition> distSpeedPoints; // liste des points de la vitesse par rapport à la distance
            QList<IndexedPosition> timeSpeedPoints; // Liste des points de la vitesse par rapport au temps
            QList<IndexedPosition> dAccPoints;      // Liste des points de l'accélération par rapport à la distance
            QList<IndexedPosition> tAccPoints;      // Liste des points de l'accélération par rapport au temps

            QVector<QPointF> distanceSpeedPoints;

            QPointF lastSpeed(0,0); // Modifié
            int count = 0;
            double lastPos = 1.5;

            while (speedQuery.next())
            {
                //qDebug() << "timestamp = " << speedQuery.value(0).toFloat();

                double time = speedQuery.value(0).toFloat() / 1000; // Le temps est sauvé en millisecondes dans la db et on le veut en secondes
                double speed = speedQuery.value(1).toDouble();
                double pos;

                IndexedPosition dPoint, tPoint;
                dPoint.setIndex(time);
                tPoint.setIndex(time);

//                if (count > 0)
//                {
                    qreal diff = (speed - lastSpeed.y()) / 3.6; // vitesse en m/s
                    qreal acc = diff / (time - lastSpeed.x());

//                    if (qAbs(acc) < 2 && (time - lastSpeed.x()) < 1)
//                    {
                        int multipleWheelPerimeter = ceil(((speed + lastSpeed.y()) / (2 * 3.6)) * (time - lastSpeed.x())) / 1.5;
                        pos = lastPos + multipleWheelPerimeter * 1.5;

                        //qDebug() << "multipleWheelPerimeter = " << multipleWheelPerimeter;

                        dPoint.setX(pos);
                        dPoint.setY(speed);
                        lastPos = pos;



                        tPoint.setX(time);
                        tPoint.setY(speed);
                        timeSpeedPoints << tPoint;

                        dAccPoints << IndexedPosition((pos + lastPos) / 2, acc, time);
                        tAccPoints << IndexedPosition((lastSpeed.x() + time) / 2, acc, time);

                        distanceSpeedPoints << QPointF(pos, speed);

//                    }
//                    else
//                    {
//                        qDebug() << "count : " << count;
//                        dPoint.setX(lastPos);
//                        dPoint.setY(lastSpeed.y());
//                        tPoint.setX(time);
//                        tPoint.setY(lastSpeed.y());
//                        timeSpeedPoints << tPoint;
//                    }
//                }
//                else
//                {
//                    dPoint.setX(0);
//                    dPoint.setY(speed);
//                }

                distSpeedPoints << dPoint;
                lastSpeed = QPointF(time, speed);
                count++;
            }

            /*
            QList<QPointF> lineZero;

            if (! dAccPoints.isEmpty()) {
                lineZero << QPointF(dAccPoints.first().x(), 50) << QPointF(dAccPoints.last().x(), 50);
                PlotCurve* dc = distancePlotFrame->addCurve(dAccPoints, trackIdentifier);
                dc->translate(0, 50);
                dc->scale(1, 5);
                distancePlotFrame->addCurve(lineZero);

                lineZero.clear();
                lineZero << QPointF(tAccPoints.first().x(), 50) << QPointF(tAccPoints.last().x(), 50);
                PlotCurve* tc = timePlotFrame->addCurve(tAccPoints, trackIdentifier);
                tc->translate(0, 50);
                tc->scale(1, 5);
                timePlotFrame->addCurve(lineZero);
            }
*/
            this->distancePlotFrame->scene()->addCurve(distSpeedPoints, trackIdentifier);//this->distancePlotFrame->addCurve(distSpeedPoints, trackIdentifier);
            this->timePlotFrame->scene()->addCurve(timeSpeedPoints, trackIdentifier);//this->timePlotFrame->addCurve(timeSpeedPoints, trackIdentifier);

//            bool accelerationPositiv = false;

//            foreach (const IndexedPosition& ip, dAccPoints) {
//                qreal value = (ip.y() - 50) / 5;
//                qDebug() << value;
//                if (value > 0 && ! accelerationPositiv) {
//                    qDebug() << "+ transition found";
//                    mapFrame->scene()->fixSymbol(ip.index(), QColor(0, 255, 0, 150), trackIdentifier);
//                    accelerationPositiv = true;
//                } else if (value < 0 && accelerationPositiv) {
//                    qDebug() << "- transition found";
//                    mapFrame->scene()->fixSymbol(ip.index(), QColor(203, 0, 0, 150), trackIdentifier);
//                    accelerationPositiv = false;
//                }
//            }
//            if (tAccPoints.size() > 2) {
//                CurvePathBuilder builder(tAccPoints.at(0), tAccPoints.at(1), 1);

//                for (int i = 2; i < tAccPoints.size(); i++)
//                    builder.append(tAccPoints.at(i));
//                timePlotFrame->scene()->addPath(builder.exBound());
//            }

            /*
            QList<QVariant> raceInformation;
            for (int i(0); i < tAccPoints.count(); i++)
            {
                raceInformation.clear();

                // ajout des informations
                raceInformation.append("Course " + QString::number(ref_race) + " tour " + QString::number(ref_lap)); // course
                raceInformation.append(timeSpeedPoints.at(i).index()); // tps 1
                raceInformation.append(timeSpeedPoints.at(i).index()); // tps 2
                raceInformation.append(distSpeedPoints.at(i).x()); // distance
                raceInformation.append(timeSpeedPoints.at(i).y()); // vitesse
                raceInformation.append(tAccPoints.at(i).y()); // Acceleration
                raceInformation.append("RPM"); // RPM
                raceInformation.append("PW"); // PW

                this->raceInformationTableModel->addRaceInformation(raceInformation);
            }
            */

            qDebug() << "Nb lignes retournées par la requete = " << count;
            qDebug() << "distSpeedPoints.count() = " << distSpeedPoints.count();
            qDebug() << "timeSpeedPoints.count() = " << timeSpeedPoints.count();
            qDebug() << "dAccPoints.count() = " << dAccPoints.count();
            qDebug() << "tAccPoints.count() = " << tAccPoints.count();

//            QList<QVariant> raceInformation;
//            for (int i(0); i < tAccPoints.count(); i++) // Environ 1000 éléments
//            {
//                raceInformation.clear();

//                // ajout des informations
//                raceInformation.append(QVariant());
//                raceInformation.append(timeSpeedPoints.at(i).index()); // tps 1
//                raceInformation.append(timeSpeedPoints.at(i).index()); // tps 2
//                raceInformation.append(distSpeedPoints.at(i).x()); // distance
//                raceInformation.append(timeSpeedPoints.at(i).y()); // vitesse
//                raceInformation.append(tAccPoints.at(i).y()); // Acceleration
//                raceInformation.append("RPM"); // RPM
//                raceInformation.append("PW");  // PW

//                this->raceInformationTableModel->addRaceInformation(
//                            ref_race, ref_lap, raceInformation);
//            }

//            this->ui->raceTable->expandAll();

            // ------------ Remplacement des graphs par ceux de Qwt ------------
            // Create power curve
            QwtPointSeriesData* distanceSpeedSerieData = new QwtPointSeriesData(distanceSpeedPoints);
            PlotCurve2* distanceSpeedCurve = new PlotCurve2(
                        tr("Course ") + trackIdentifier["race"].toString() +
                        tr("tour ") + trackIdentifier["lap"].toString(),
                        QPen(Qt::darkRed));
            distanceSpeedCurve->setData(distanceSpeedSerieData);
            distanceSpeedCurve->attach(this->distanceSpeedPlot);
            // ------------ Remplacement des graphs par ceux de Qwt ------------
        }

    }
}

void MainWindow::connectSignals(void)
{
    // Map frame/scene
    connect(this->mapFrame->scene(), SIGNAL(sectorRemoved(QString,int)),
            this, SLOT(removeSector(QString,int)));
    connect(this->mapFrame->scene(), SIGNAL(sectorAdded(QString,int,IndexedPosition,IndexedPosition)),
            this, SLOT(addSector(QString,int,IndexedPosition,IndexedPosition)));
    connect(this->mapFrame->scene(), SIGNAL(sectorUpdated(QString,int,IndexedPosition,IndexedPosition)),
            this, SLOT(updateSector(QString,int,IndexedPosition,IndexedPosition)));
    connect(this->mapFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this->distancePlotFrame->scene(), SLOT(highlightPoints(float,QVariant)));
    connect(this->mapFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this->timePlotFrame->scene(), SLOT(highlightPoints(float,QVariant)));
    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this->distancePlotFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this->timePlotFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
    connect(this->mapFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this, SLOT(displayLapInformation(float,float,QVariant)));
//    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
//            this->distancePlotFrame->scene(), SLOT(clearPlotSelection()));
//    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
//            this->timePlotFrame->scene(), SLOT(clearPlotSelection()));
    connect(this->mapFrame->scene(), SIGNAL(selectionChanged()),
            this, SLOT(on_actionLapDataEraseTable_triggered()));
    connect(this->mapFrame, SIGNAL(clearTracks()),
            this, SLOT(on_actionClearAllData_triggered()));

    // Distance plot frame/scene
    //connect(this->distancePlotFrame, SIGNAL(selectionChanged()),
            //this->mapFrame->scene(), SLOT(clearSceneSelection()));
    connect(this->distancePlotFrame->scene(), SIGNAL(selectionChanged()),
            this, SLOT(on_actionLapDataEraseTable_triggered()));
    connect(this->distancePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this->mapFrame->scene(), SLOT(highlightPoint(float,QVariant)));
    connect(this->distancePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this, SLOT(displayLapInformation(float,QVariant)));
    connect(this->distancePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this->mapFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
    connect(this->distancePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this, SLOT(displayLapInformation(float,float,QVariant)));
    connect(this->distancePlotFrame, SIGNAL(clear()),
            this, SLOT(on_actionClearAllData_triggered()));

    // Time plot frame/scene
    //connect(this->timePlotFrame, SIGNAL(selectionChanged()),
            //this->mapFrame->scene(), SLOT(clearSceneSelection()));
    connect(this->timePlotFrame->scene(), SIGNAL(selectionChanged()),
            this, SLOT(on_actionLapDataEraseTable_triggered()));
    connect(this->timePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this->mapFrame->scene(), SLOT(highlightPoint(float,QVariant)));
    connect(this->timePlotFrame->scene(), SIGNAL(pointSelected(float,QVariant)),
            this, SLOT(displayLapInformation(float,QVariant)));
    connect(this->timePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this->mapFrame->scene(), SLOT(highlightSector(float,float,QVariant)));
    connect(this->timePlotFrame->scene(), SIGNAL(intervalSelected(float,float,QVariant)),
            this, SLOT(displayLapInformation(float,float,QVariant)));
    connect(this->timePlotFrame, SIGNAL(clear()),
            this, SLOT(on_actionClearAllData_triggered()));
}

void MainWindow::reloadRaceView(void)
{
    QSqlQueryModel* model = new QSqlQueryModel(this);
    QSqlQuery getAllLaps;
    getAllLaps.prepare("select date, race.num, lap.num, race.id, lap.num "
                       "from COMPETITION, RACE, LAP "
                       "where COMPETITION.name = ? and COMPETITION.name = RACE.ref_compet and RACE.id = LAP.ref_race "
                       "order by RACE.date, RACE.num, LAP.num");
    getAllLaps.addBindValue(this->currentCompetition);

    if (!getAllLaps.exec())
        return;

    model->setQuery(getAllLaps);

    this->competitionModel = new GroupingTreeModel(this);
    QList<int> cols;
    cols << 0 << 1; // ce par quoi on va grouper les éléments dans la vue (première colonne)
    this->competitionModel->setSourceModel(model, cols);

    CompetitionProxyModel* wrapper = new CompetitionProxyModel(this);
    wrapper->setSourceModel(this->competitionModel);

    QItemSelectionModel* oldSelectionModel = this->ui->raceView->selectionModel();
    if (oldSelectionModel != NULL)
        delete oldSelectionModel;

    this->ui->raceView->setModel(wrapper);
    this->ui->raceView->expandAll();
    this->ui->raceView->resizeColumnToContents(0);

    /* Do not show race.id or lap.num through view*/
    this->ui->raceView->setColumnHidden(1, true);
    this->ui->raceView->setColumnHidden(2, true);

//    connect(this->ui->raceView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(competitionSelection(QItemSelection,QItemSelection)));
}

void MainWindow::loadSectors(const QString &competitionName)
{
    this->sectorModel = new QSqlTableModel(this);
    this->sectorModel->setTable("SECTOR");
    this->sectorModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    this->sectorModel->setFilter(QString("ref_compet = '%1'").arg(competitionName));
    this->sectorModel->setSort(this->sectorModel->fieldIndex("num"), Qt::AscendingOrder);
    this->sectorModel->setHeaderData(3, Qt::Horizontal, tr("Vmin (km/h)"));
    this->sectorModel->setHeaderData(4, Qt::Horizontal, tr("Vmax (km/h)"));
    this->sectorModel->select();

    ColorizerProxyModel* coloredModel = new ColorizerProxyModel(6, this);
    coloredModel->setSourceModel(this->sectorModel);
    coloredModel->setIndexColumn(1);
    this->ui->sectorView->setModel(coloredModel);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("id"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("num"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("ref_compet"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("start_pos"), true);
    this->ui->sectorView->setColumnHidden(this->sectorModel->fieldIndex("end_pos"), true);
    this->ui->sectorView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QSqlQuery posQuery;
    posQuery.prepare("select longitude, latitude, id from POSITION where id >= ? and id <= ?");
    int currentSector = 0;
    int startPosInd = this->sectorModel->fieldIndex("start_pos");
    int endPosInd = this->sectorModel->fieldIndex("end_pos");

    qDebug() << "sectors nb ; " << this->sectorModel->rowCount()  ;
    while (currentSector < this->sectorModel->rowCount())
    {
        posQuery.bindValue(0, this->sectorModel->record(currentSector).value(startPosInd));
        posQuery.bindValue(1, this->sectorModel->record(currentSector).value(endPosInd));

        if (posQuery.exec())
        {
            QVector<IndexedPosition> sectorPoints;

            while (posQuery.next())
            {
                GeoCoordinate tmp;
                tmp.setLongitude(posQuery.value(0).toFloat());
                tmp.setLatitude(posQuery.value(1).toFloat());
                sectorPoints.append(IndexedPosition(tmp.projection(), posQuery.value(2).toInt()));
            }

            if (! sectorPoints.isEmpty())
                this->mapFrame->scene()->addSector(sectorPoints, competitionName);
        }
        currentSector++;
    }

    this->ui->sectorView->setVisible(mapFrame->scene()->hasSectors());
}

void MainWindow::highlightPointInAllView(const QModelIndex &index)
{
    QModelIndex LapNumModelIndex;
    QModelIndex RaceNumModelIndex;

    // Get lap and race num
    LapNumModelIndex  = index.parent();
    RaceNumModelIndex = LapNumModelIndex.parent();

    QMap<QString, QVariant> trackId;
    trackId["race"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(RaceNumModelIndex.row(),
                                                       0)).toInt();

    trackId["lap"] = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(LapNumModelIndex.row(),
                                                 0, RaceNumModelIndex)).toInt();

    // Get the time in milliseconds for the selected item
    float time = this->raceInformationTableModel->data(
                this->raceInformationTableModel->index(
                    index.row(), 1, LapNumModelIndex)).toInt() / 1000.0;

    // Highlight point in all view
    this->mapFrame->scene()->highlightPoint(time, trackId);
    this->timePlotFrame->scene()->highlightPoint(time, trackId);
    this->distancePlotFrame->scene()->highlightPoint(time, trackId);
}

Plot* MainWindow::currentPlot(void) const
{
    switch (this->ui->plotTabWidget->currentIndex())
    {
    case 4:

        return this->timeSpeedPlot;
    default:
        return this->distanceSpeedPlot;
    }
}

void MainWindow::createPlotLegendContextMenu(void)
{
    // Legend actions
    this->legendContextMenu = new QMenu(this);
    this->legendContextMenu->addAction(
                tr("Effacer"),this, SLOT(eraseCurve()));
    this->legendContextMenu->addAction(
                tr("Centrer sur"), this, SLOT(centerOnCurve()));
    this->legendContextMenu->addAction(
                tr("Changer la couleur"), this, SLOT(changeCurveColor()));
    this->legendContextMenu->addAction(
                tr("Ajouter une courbe de tendance polynomiale"), this,
                SLOT(createPolynomialTrendline()));
}

void MainWindow::createPlotsZone(void)
{
    // Création du graphique Distance(x) - Vitesse(y)
    this->distanceSpeedPlot = new Plot(tr("Distance - Vitesse"), this);
    this->distanceSpeedPlot->setAxisTitle(Plot::xBottom, tr("Distance (m)"));
    this->distanceSpeedPlot->setAxisTitle(Plot::yLeft, tr("Vitesse (km/h)"));
    this->ui->distanceSpeedVerticalLayout->addWidget(this->distanceSpeedPlot);

    connect(this->distanceSpeedPlot, SIGNAL(legendChecked(QwtPlotItem*,bool)),
            this, SLOT(setPlotCurveVisibile(QwtPlotItem*,bool)));
    connect(this->distanceSpeedPlot, SIGNAL(legendRightClicked(const QwtPlotItem*,QPoint)),
            this, SLOT(showLegendContextMenu(const QwtPlotItem*,QPoint)));

    // Création du graphique Temps(x) - Vitesse(y)
    this->timeSpeedPlot = new Plot(tr("Temps - Vitesse"), this);
    this->timeSpeedPlot->setAxisTitle(Plot::xBottom, tr("Temps (s)"));
    this->timeSpeedPlot->setAxisTitle(Plot::yLeft, tr("Vitesse (km/h)"));
    this->ui->timeSpeedVerticalLayout->addWidget(this->timeSpeedPlot);

    connect(this->timeSpeedPlot, SIGNAL(legendChecked(QwtPlotItem*,bool)),
            this, SLOT(setPlotCurveVisibile(QwtPlotItem*,bool)));
    connect(this->timeSpeedPlot, SIGNAL(legendRightClicked(const QwtPlotItem*,QPoint)),
            this, SLOT(showLegendContextMenu(const QwtPlotItem*,QPoint)));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Save the state of the mainWindow and its widgets
    this->writeSettings("MainWindow");

    QMainWindow::closeEvent(event);
}

void MainWindow::on_showLineToolButton_toggled(bool checked)
{
    // Récupérer la liste de toutes les courbes dans le graphique courant
    QwtPlotItemList curvesList = this->currentPlot()->itemList();

    qDebug() << "Nombre d'éléments dans le graphique : " << curvesList.count();

    foreach (QwtPlotItem* item, curvesList)
    {
        if (!(item->rtti() == QwtPlotItem::Rtti_PlotCurve))
            continue;

        PlotCurve2* curve = (PlotCurve2*) item;

        if(curve == NULL)
            continue;

        qDebug() << "Avant ...";
        if (checked)
            curve->setStyle(QwtPlotCurve::Lines);
        else
            curve->setStyle(PlotCurve2::Dots);
        qDebug() << "Après ...";
    }
}

void MainWindow::on_showCrossToolButton_toggled(bool checked)
{
    this->currentPlot()->setCrossLineVisible(checked);
}
