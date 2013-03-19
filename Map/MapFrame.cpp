#include "MapFrame.hpp"
#include "ui_MapFrame.h"

MapFrame::MapFrame(QWidget *parent) :
    QFrame(parent), ui(new Ui::MapFrame), mapScene(NULL), mapView(NULL)
{
    // GUI Configuration
    ui->setupUi(this);

    // Create scene
    this->mapScene = new MapScene(50 * 1000, this);

    // Create view
    this->mapView = new MapView(this->mapScene, this);
    this->mapView->setRenderHint(QPainter::Antialiasing, true);
    this->mapView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    this->mapView->setDragMode(QGraphicsView::ScrollHandDrag);

    // Add the view to the view layout
    this->ui->viewHorizontalLayout->insertWidget(0, this->mapView);

    // Connect signals and slots of view and scene
    connect(this->mapView, SIGNAL(areaDelimited(QPointF,QPointF)),
            this->mapScene, SLOT(cutSectorBetween(QPointF,QPointF)));
    connect(this->mapView, SIGNAL(pointOrZoneSelected()),
            this->mapScene, SLOT(manageSelectedZone()));
    connect(this->mapView, SIGNAL(zoomedAround(int, QPointF)),
            this, SLOT(zoomAround(int,QPointF)));
}

MapFrame::~MapFrame(void)
{
    delete this->ui;
    delete this->mapScene;
    delete this->mapView;
}

MapView* MapFrame::view(void) const
{
    return this->mapView;
}

MapScene* MapFrame::scene(void) const
{
    return this->mapScene;
}

// updateScale
void MapFrame::on_zoomSlider_valueChanged(int value)
{
    qreal scaleFactor =  qPow(2, (value - 50) / 10.0); //qreal scaleFactor =  qPow(2, (this->ui->zoomSlider->value() - 50) / 10.0);

    this->mapView->resetMatrix();
    this->mapView->scale(scaleFactor, scaleFactor);
}

void MapFrame::on_selectPointZoneButton_toggled(bool checked)
{
    // Active the RubberBandDrag and change the cursor
    if (checked)
        this->mapView->setDragMode(QGraphicsView::RubberBandDrag);

    this->mapScene->enableTrackAcceptHoverEvents(checked); // Appeler le slot directement ou faire un connect en plus de l'autoconnect ??? (surtout, est ce que si je fais un connect avec cet objet, l'autoconnect marchera t-il encore ?
}

void MapFrame::on_cutSectorButton_toggled(bool checked)
{
    if (checked)
        this->mapView->setDragMode(QGraphicsView::NoDrag);
}

void MapFrame::on_dragButton_toggled(bool checked)
{
    if (checked)
        this->mapView->setDragMode(QGraphicsView::ScrollHandDrag);
}

void MapFrame::on_clearToolButton_clicked(void)
{
    emit this->clearTracks();
}

void MapFrame::on_eraseSelectionToolButton_clicked(void)
{
    this->mapScene->clearSceneSelection();
}

void MapFrame::zoom(int level)
{
    this->ui->zoomSlider->setValue(this->ui->zoomSlider->value() + level * 2);
}

void MapFrame::zoomAround(int level, QPointF origin)
{
    this->mapView->centerOn(this->mapView->mapToScene(origin.toPoint()));
    this->zoom(level);
}
