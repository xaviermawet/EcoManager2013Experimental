#include "LapDataCompartor.hpp"
#include "ui_LapDataCompartor.h"

LapDataCompartor::LapDataCompartor(int raceNumber, int lapNumber,
                                   QWidget *parent) :
    QDialog(parent), ui(new Ui::LapDataCompartor),
    raceNum(raceNumber), lapNum(lapNumber)
{
    // GUI Configuration
    ui->setupUi(this);

    // Building Model
    this->createTableModel();

    QString title = "Comparaison des données pour le tour "
                  + QString::number(lapNumber) + " de la course "
                  + QString::number(raceNumber);
    this->setWindowTitle(title);
}

LapDataCompartor::~LapDataCompartor(void)
{
    delete this->ui;
    delete this->lapDataTableModel;
}

void LapDataCompartor::addLapsData(const QList<QVariant>& dataLap1,
                                   const QList<QVariant>& dataLap2)
{
    // Remove laps information from the table
    this->lapDataTableModel->removeRows(0, this->lapDataTableModel->rowCount());

    // Add the laps data to the table
    QList< QList<QVariant> > lapsData;

    if (dataLap1.at(TIME_S_COLUMN).toFloat()
            <= dataLap2.at(TIME_S_COLUMN).toFloat())
    {
        lapsData.append(dataLap1);
        lapsData.append(dataLap2);

        // time calculation
        this->ui->timeDifferenceMsDoubleSpinBox->setValue(
                    dataLap2.at(TIME_MS_COLUMN).toInt()
                    - dataLap1.at(TIME_MS_COLUMN).toInt());
        this->ui->timeDifferenceSDoubleSpinBox->setValue(
                    dataLap2.at(TIME_S_COLUMN).toDouble()
                    - dataLap1.at(TIME_S_COLUMN).toDouble());

        // distance calculation
        this->ui->distanceDifferenceDoubleSpinBox->setValue(
                    dataLap2.at(DISTANCE_COLUMN).toDouble()
                    - dataLap1.at(DISTANCE_COLUMN).toDouble());

        // Speed calculation
        this->ui->speedDifferenceDoubleSpinBox->setValue(
                    dataLap2.at(SPEED_COLUMN).toDouble()
                    - dataLap1.at(SPEED_COLUMN).toDouble());
    }
    else
    {
        lapsData.append(dataLap2);
        lapsData.append(dataLap1);

        // time calculation
        this->ui->timeDifferenceMsDoubleSpinBox->setValue(
                    dataLap1.at(TIME_MS_COLUMN).toInt()
                    - dataLap2.at(TIME_MS_COLUMN).toInt());
        this->ui->timeDifferenceSDoubleSpinBox->setValue(
                    dataLap1.at(TIME_S_COLUMN).toDouble()
                    - dataLap2.at(TIME_S_COLUMN).toDouble());

        // distance calculation
        this->ui->distanceDifferenceDoubleSpinBox->setValue(
                    dataLap1.at(DISTANCE_COLUMN).toDouble()
                    - dataLap2.at(DISTANCE_COLUMN).toDouble());

        // Speed calculation
        this->ui->speedDifferenceDoubleSpinBox->setValue(
                    dataLap1.at(SPEED_COLUMN).toDouble()
                    - dataLap2.at(SPEED_COLUMN).toDouble());
    }

    // Calcul de l'accélération
    qreal diff = this->ui->speedDifferenceDoubleSpinBox->value() / 3.6; // La vitesse est exprimée en km\h et on la veut en m/s --> diviser en 3.6
    qreal acc  = diff / this->ui->timeDifferenceSDoubleSpinBox->value();
    this->ui->accelerationDoubleSpinBox->setValue(acc);

    this->lapDataTableModel->addMultipleRaceInformation(
                this->raceNum, this->lapNum, lapsData);

    this->ui->lapDataTreeView->expandAll();
}

void LapDataCompartor::createTableModel(void)
{
    // Create the model for the table of laps information
    QStringList headers;
    headers << tr("Course") << tr("Tps(ms)") << tr("Tps(s)") << tr("Dist(m)")
            << tr("v(km\\h)") << tr("Acc (m\\s2)") << tr("RPM") << tr("PW");
    this->lapDataTableModel = new LapInformationTreeModel(headers);

    /* Use a proxy model to manage background color of each row and manage
     * how the data for Race and lap number are displayed */
    LapInformationProxyModel* wrapper = new LapInformationProxyModel(this);
    wrapper->setSourceModel(this->lapDataTableModel);

    // Apply the model to the table and change the selection mode
    this->ui->lapDataTreeView->setModel(wrapper);
    this->ui->lapDataTreeView->setSelectionMode(QAbstractItemView::NoSelection);
    this->ui->lapDataTreeView->setAlternatingRowColors(true);
    this->ui->lapDataTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
}
