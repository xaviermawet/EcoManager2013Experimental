#include "MapFrame.hpp"
#include "ui_MapFrame.h"

MapFrame::MapFrame(QWidget *parent) :
    QFrame(parent), ui(new Ui::MapFrame), _view(NULL)
{
    // GUI Configuration
    ui->setupUi(this);

    // View Configuration
    this->_view = new MapView(this);
    this->_view->setRenderHint(QPainter::Antialiasing, true);
    this->_view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    this->_view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Add the view to the view layout
    this->ui->viewHorizontalLayout->insertWidget(0, this->_view);

    // Connect the view signal to MapFrame
    connect(this->_view, SIGNAL(zoomedAround(int, QPointF)),
            this, SLOT(zoomAround(int,QPointF)));
}

MapFrame::~MapFrame(void)
{
    delete ui;
}

MapView* MapFrame::view(void) const
{
    return this->_view;
}

// updateScale
void MapFrame::on_zoomSlider_valueChanged(int value)
{
    Q_UNUSED(value);

    qreal scaleFactor =  qPow(2, (this->ui->zoomSlider->value() - 50) / 10.0);

    this->_view->resetMatrix();
    this->_view->scale(scaleFactor, scaleFactor);
}

void MapFrame::on_selectPointZoneButton_toggled(bool checked)
{
    // Active the RubberBandDrag and change the cursor
    if (checked)
        this->_view->setDragMode(QGraphicsView::RubberBandDrag);

    emit this->enableTrackHoverEvent(checked);
}

void MapFrame::on_cutSectorButton_toggled(bool checked)
{
    if (checked)
        this->_view->setDragMode(QGraphicsView::NoDrag);
}

void MapFrame::on_dragButton_toggled(bool checked)
{
    if (checked)
        this->_view->setDragMode(QGraphicsView::ScrollHandDrag);
}

void MapFrame::on_clearToolButton_clicked(void)
{
    emit this->clearTracks();
}

void MapFrame::zoom(int level)
{
    this->ui->zoomSlider->setValue(this->ui->zoomSlider->value() + level * 2);
}

void MapFrame::zoomAround(int level, QPointF origin)
{
    this->_view->centerOn(this->_view->mapToScene(origin.toPoint()));
    this->zoom(level);
}

void MapFrame::on_eraseSelectionToolButton_clicked(void)
{
    QMessageBox::information(this, "Impossible", "Ne sais rien faire car ne contient pas la scene ... quel con");
}
