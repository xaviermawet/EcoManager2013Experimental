#include "DoubleYAxisPlot.hpp"

DoubleYAxisPlot::DoubleYAxisPlot(
        const QString& title, double aspectRatio, QWidget* parent) :
    DoubleYAxisPlot(QwtText(title), aspectRatio, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

DoubleYAxisPlot::DoubleYAxisPlot(
        const QwtText& title, double aspectRatio, QWidget* parent) :
    AbstractDoubleAxisPlot(title, parent), _ratio(aspectRatio), _yRescaler(NULL)
{
    /* ---------------------------------------------------------------------- *
     *                       Add a y axis on the right                        *
     * ---------------------------------------------------------------------- */

    this->enableAxis(yRight);

    /* ---------------------------------------------------------------------- *
     *       Rescaler takes care of fixed aspect ratios for plot scales       *
     * ---------------------------------------------------------------------- */
    this->_yRescaler = new QwtPlotRescaler(this->canvas());
    this->_yRescaler->setReferenceAxis(yLeft);
    this->_yRescaler->setAspectRatio(yRight, this->_ratio);
    this->_yRescaler->setAspectRatio(xBottom, 0.0);
    this->_yRescaler->setAspectRatio(xTop, 0.0);
}

DoubleYAxisPlot::~DoubleYAxisPlot(void)
{
    qDebug() << "DoubleYAxisPlot (" << this->objectName() << ") Début destructeur";

    delete this->_yRescaler;

    qDebug() << "DoubleYAxisPlot (" << this->objectName() << ") Fin destructeur";
}

void DoubleYAxisPlot::zoom(const QwtPlotItem* item)
{
    QRectF rect(item->boundingRect());
    QPointF topLeft(rect.topLeft());
    QPointF bottomRight(rect.bottomRight());

    if (item->yAxis() == yLeft)
    {
        this->_xBottomYLeftZoomer->zoom(rect);

        topLeft.setY(topLeft.y() / this->_ratio);
        bottomRight.setY(bottomRight.y() / this->_ratio);
        this->_xTopYRightZoomer->zoom(QRectF(topLeft, bottomRight));
    }
    else
    {
        this->_xTopYRightZoomer->zoom(rect);

        topLeft.setY(topLeft.y() * this->_ratio);
        bottomRight.setY(bottomRight.y() * this->_ratio);
        this->_xBottomYLeftZoomer->zoom(QRectF(topLeft, bottomRight));
    }

    //this->_magnifier->restoreScale();
}
