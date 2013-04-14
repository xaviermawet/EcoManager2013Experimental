#include "DoubleXAxisPlot.hpp"

DoubleXAxisPlot::DoubleXAxisPlot(
        const QString &title, double aspectRatio, QWidget *parent) :
    DoubleXAxisPlot(QwtText(title), aspectRatio, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

DoubleXAxisPlot::DoubleXAxisPlot(
        const QwtText &title, double aspectRatio, QWidget *parent) :
    AbstractDoubleAxisPlot(title, parent), _ratio(aspectRatio), _xRescaler(NULL)
{
    /* ---------------------------------------------------------------------- *
     *                        Add a x axis on the top                         *
     * ---------------------------------------------------------------------- */

    this->enableAxis(xTop);

    /* ---------------------------------------------------------------------- *
     *       Rescaler takes care of fixed aspect ratios for plot scales       *
     * ---------------------------------------------------------------------- */
    this->_xRescaler = new QwtPlotRescaler(this->canvas());
    this->_xRescaler->setReferenceAxis(xBottom);
    this->_xRescaler->setAspectRatio(xTop, this->_ratio);
    this->_xRescaler->setAspectRatio(yRight, 0.0);
    this->_xRescaler->setAspectRatio(yLeft, 0.0);
}

DoubleXAxisPlot::~DoubleXAxisPlot(void)
{
    qDebug() << "DoubleXAxisPlot (" << this->objectName() << ") Début destructeur";

    delete this->_xRescaler;

    qDebug() << "DoubleXAxisPlot (" << this->objectName() << ") Fin destructeur";
}

void DoubleXAxisPlot::zoom(const QwtPlotItem *item)
{
    QRectF rect(item->boundingRect());
    QPointF topLeft(rect.topLeft());
    QPointF bottomRight(rect.bottomRight());

    if (item->xAxis() == xBottom)
    {
        this->_xBottomYLeftZoomer->zoom(rect);

        topLeft.setX(topLeft.x() / this->_ratio);
        bottomRight.setX(bottomRight.x() / this->_ratio);
        this->_xTopYRightZoomer->zoom(QRectF(topLeft, bottomRight));
    }
    else
    {
        this->_xTopYRightZoomer->zoom(rect);

        topLeft.setX(topLeft.x() * this->_ratio);
        bottomRight.setX(bottomRight.x() * this->_ratio);
        this->_xBottomYLeftZoomer->zoom(QRectF(topLeft, bottomRight));
    }

    //this->_magnifier->restoreScale();
}
