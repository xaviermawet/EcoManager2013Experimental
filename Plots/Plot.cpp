#include "Plot.hpp"

Plot::Plot(const QString& title, QWidget* parent) :
    Plot(QwtText(title), parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

Plot::Plot(const QwtText& title, QWidget* parent) :
    QwtPlot(title, parent), _legend(NULL), _grid(NULL),
    _crossLine(NULL), _panner(NULL), _xBottomYLeftZoomer(NULL),
    _magnifier(NULL), _labelWasVisible(false)
{
    this->setAutoReplot(false);

    /* ---------------------------------------------------------------------- *
     *                         Add a legend for curves                        *
     * ---------------------------------------------------------------------- */
    this->_legend = new QwtLegend(this);
    this->_legend->setItemMode(QwtLegend::CheckableItem);
    this->_legend->setContextMenuPolicy(Qt::CustomContextMenu);
    this->insertLegend(this->_legend, QwtPlot::BottomLegend);

    connect(this->_legend, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showLegendContextMenu(QPoint)));

    /* ---------------------------------------------------------------------- *
     *                                Add a grid                              *
     * ---------------------------------------------------------------------- */
    this->_grid = new QwtPlotGrid();
    this->_grid->setTitle(tr("Grille"));
    this->_grid->setMajPen(QPen(Qt::gray, Qt::SolidLine));
    this->_grid->attach(this);

    /* ---------------------------------------------------------------------- *
     *                            Add a cross marker                          *
     * ---------------------------------------------------------------------- */
    this->_crossLine = new QwtPlotMarker();
    this->_crossLine->setLineStyle(QwtPlotMarker::Cross);
    this->_crossLine->setValue(0, 0);
    this->_crossLine->attach(this);

    /* ---------------------------------------------------------------------- *
     *                      Manage panning for the plot                       *
     * ---------------------------------------------------------------------- *
     * MidButton for the panning                                              *
     * ---------------------------------------------------------------------- */
    this->_panner = new QwtPlotPanner(this->canvas());
    this->_panner->setMouseButton(Qt::MidButton);

    /* ---------------------------------------------------------------------- *
     *                         Manage zoom for the plot                       *
     * ---------------------------------------------------------------------- *
     * Left button               : dragging rect                              *
     * Right button              : zoom out by 1                              *
     * Ctrl + Righ button        : zoom out to full size                      *
     * Mouse wheel or Ctrl + +/- : zoom in/out by 1                           *
     * ---------------------------------------------------------------------- */
    this->_xBottomYLeftZoomer = new Zoomer(this->canvas());

    // Display coordinates at mouse position every time
    this->_xBottomYLeftZoomer->setTrackerMode(QwtPicker::AlwaysOn);

    // Allow to zoom with left button by dragging a rect
    this->_xBottomYLeftZoomer->setRubberBand(QwtPicker::RectRubberBand);


    // Manage zoom with the mouse wheel and keyborad
    this->_magnifier = new PlotMagnifier(this->canvas());
    this->_magnifier->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);
    this->_magnifier->setZoomInKey(Qt::Key_Plus,
                                  Qt::ControlModifier | Qt::ShiftModifier);

    /* ---------------------------------------------------------------------- *
     *                      Some customization options                        *
     * ---------------------------------------------------------------------- */
    this->setCanvasBackground(Qt::darkGray);

    this->setAutoReplot(true);
}

Plot::~Plot(void)
{
    qDebug() << "Plot (" << this->objectName() << ") Début destructeur";

    delete this->_legend;
    delete this->_grid;
    delete this->_crossLine;
    delete this->_panner;
    delete this->_xBottomYLeftZoomer;
    delete this->_magnifier;

    qDebug() << "Plot (" << this->objectName() << ") fin destructeur";
}

void Plot::zoom(const QRectF& zoomRectF)
{
    this->_magnifier->restoreScale();
    this->_xBottomYLeftZoomer->zoom(zoomRectF);
}

void Plot::zoom(const QwtPlotItem* item)
{
    qDebug() << "Je suis un zoom normal ... je n'ai qu'un zoomer...";

    this->zoom(item->boundingRect());
}

bool Plot::isGridVisible(void) const
{
    return this->_grid->plot() != NULL;
}

bool Plot::isCrossLineVisible(void) const
{
    return this->_crossLine->plot() != NULL;
}

bool Plot::isLabelPositionVisible(void) const
{
    return this->_xBottomYLeftZoomer->trackerMode() == QwtPlotPicker::AlwaysOn;
}

QwtPlotGrid* Plot::grid(void) const
{
    return this->_grid;
}

QwtPlotMarker* Plot::crossLine(void) const
{
    return this->_crossLine;
}

Zoomer* Plot::zoomer(void) const
{
    return this->_xBottomYLeftZoomer;
}

void Plot::setGridVisible(bool visible)
{
    visible ? this->_grid->attach(this) : this->_grid->detach();
    this->replot();
}

void Plot::setCrossLineVisible(bool visible)
{
    if (visible)
    {
        // Change cross line visibility
        this->_crossLine->attach(this);
        connect(this->_xBottomYLeftZoomer, SIGNAL(mousePosChanged(QPointF)),
                this, SLOT(updateCrossLinePosition(QPointF)));

        // Save the previous label visibility
        this->_labelWasVisible = this->isLabelPositionVisible();
        this->setLabelPositionVisible(true);
    }
    else
    {
        // Change cross line visibility
        this->_crossLine->detach();
        disconnect(this->_xBottomYLeftZoomer, SIGNAL(mousePosChanged(QPointF)),
                   this, SLOT(updateCrossLinePosition(QPointF)));

        // Restore the previous label visibility
        this->setLabelPositionVisible(this->_labelWasVisible);
    }

    this->replot();
}

void Plot::setLabelPositionVisible(bool visible)
{
    if (visible)
    {
        this->_xBottomYLeftZoomer->setTrackerMode(QwtPicker::AlwaysOn);
    }
    else
    {
        if (this->isCrossLineVisible())
            return;

        this->_xBottomYLeftZoomer->setTrackerMode(QwtPicker::AlwaysOff);
    }

    this->replot();
}

void Plot::updateCrossLinePosition(const QPointF& pos)
{
    this->_crossLine->setValue(pos);
}

void Plot::showLegendContextMenu(const QPoint& pos)
{
    // Stop if the user doesn't right clic on a legend item
    QwtLegendItem* legendItem = qobject_cast<QwtLegendItem*>(
                this->_legend->childAt(pos));
    if (legendItem == NULL)
        return;

    foreach(QwtPlotItem* plotItem, this->itemList())
    {
        if (plotItem->title().text() == legendItem->text().text())
        {
            QPoint globalPos = this->_legend->mapToGlobal(pos);
            emit this->legendRightClicked(plotItem, globalPos);
            break;
        }
    }
}
