#include "SampleLapViewer.hpp"
#include "ui_SampleLapViewer.h"

SampleLapViewer::SampleLapViewer(const QString& referencedCompetition,
                                 QWidget *parent) :
    QDialog(parent), ui(new Ui::SampleLapViewer), _zoomLevel(1),
    _refCompetition(referencedCompetition)
{
    // GUI Configuration
    ui->setupUi(this);

    // Get races id dans laps num for the referenced competition
    this->init();
}

SampleLapViewer::~SampleLapViewer()
{
    delete ui;
}

QSize SampleLapViewer::sizeHint(void)
{
    return QSize(600, 400);
}

QPair<int, int> SampleLapViewer::selectedReferencesLap(void) const
{
    return this->_lapIds.at(this->ui->lapsViewStack->currentIndex());
}

void SampleLapViewer::on_previousLapButton_clicked(void)
{
    int currentIndex(this->ui->lapsViewStack->currentIndex());

    if (currentIndex > 0)
    {
        this->ui->lapsViewStack->setCurrentIndex(currentIndex - 1);
        this->zoomView(0);
    }
}

void SampleLapViewer::on_nextLapButton_clicked(void)
{
    int currentIndex(this->ui->lapsViewStack->currentIndex());

    if (currentIndex < this->ui->lapsViewStack->count() - 1)
    {
        this->ui->lapsViewStack->setCurrentIndex(currentIndex + 1);
    }
    else
    {
        if (currentIndex < this->_lapIds.count() - 1)
        {
            QPair<int, int> nextLap = this->_lapIds.at(currentIndex + 1);
            MapView* newView = createLapView(nextLap.first, nextLap.second);
            int widgetIndex = this->ui->lapsViewStack->addWidget(newView);
            this->ui->lapsViewStack->setCurrentIndex(widgetIndex);
        }
    }

    this->zoomView(0);
}

void SampleLapViewer::on_buttonBox_accepted(void)
{
    // Hides the modal dialog and sets the result code to Accepted.
    this->accept();
}

void SampleLapViewer::on_buttonBox_rejected(void)
{
    // Hides the modal dialog and sets the result code to Rejected.
    this->reject();
}

void SampleLapViewer::zoomView(int increment)
{
    QMatrix scaling;
    MapView* currentView = dynamic_cast<MapView*>(
                this->ui->lapsViewStack->currentWidget());

    if (currentView == NULL)
        return;

    this->_zoomLevel += increment;
    qreal scale =  qPow(2, this->_zoomLevel / 10.0);
    scaling.scale(scale, scale);
    currentView->setMatrix(scaling);
}

MapView* SampleLapViewer::createLapView(int refRace, int refLap)
{
    QVector<QPointF> positions;
    QSqlQuery posQuery;
    posQuery.prepare("select longitude, latitude from POSITION where ref_lap_race = ? and ref_lap_num = ?");
    posQuery.addBindValue(refRace);
    posQuery.addBindValue(refLap);

    if (posQuery.exec())
    {
        while (posQuery.next())
        {
            GeoCoordinate tmp;
            tmp.setLongitude(posQuery.value(0).toFloat());
            tmp.setLatitude(posQuery.value(1).toFloat());

            positions << tmp.projection();
        }
    }

    MapScene* sc = new MapScene(50 * 1000, this);
    sc->addTrack(positions);

    MapView* view = new MapView(this);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(sc);
    connect(view, SIGNAL(zoomedAround(int,QPointF)), this, SLOT(zoomView(int)));

    return view;
}

void SampleLapViewer::init(void)
{
    QSqlQuery lapsQuery;
    lapsQuery.prepare("select RACE.id, LAP.num from COMPETITION, RACE, LAP where COMPETITION.name = ? and COMPETITION.name = RACE.ref_compet and RACE.id = LAP.ref_race");
    lapsQuery.addBindValue(this->_refCompetition);

    if (lapsQuery.exec())
    {
        while (lapsQuery.next())
        {
            QPair<int, int> lap;
            lap.first = lapsQuery.value(0).toInt();
            lap.second = lapsQuery.value(1).toInt();
            this->_lapIds << lap;
        }

        this->on_nextLapButton_clicked();
    }
    else
    {
        qDebug() << lapsQuery.lastError();
    }
}
