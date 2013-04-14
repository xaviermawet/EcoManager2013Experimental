#include "AbstractDoubleAxisPlot.hpp"

AbstractDoubleAxisPlot::AbstractDoubleAxisPlot(
        const QString &title, QWidget *parent) :
    AbstractDoubleAxisPlot(QwtText(title), parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

AbstractDoubleAxisPlot::AbstractDoubleAxisPlot(
        const QwtText &title, QWidget *parent) :
    Plot(title, parent), _xTopYRightZoomer(NULL)
{
    /* ---------------------------------------------------------------------- *
     *                Add a new zoomer for the futur new axis                 *
     * ---------------------------------------------------------------------- *
     * A zoomer controls only one x and one y axis. If you want to zoom 2 x   *
     * or y axis you need a second zoomer (without tracker and rubber band)   *
     * ---------------------------------------------------------------------- */

    this->_xTopYRightZoomer = new Zoomer(xTop, yRight, this->canvas());
}

AbstractDoubleAxisPlot::~AbstractDoubleAxisPlot(void)
{
    qDebug() << "AbstractDoubleAxisPlot (" << this->objectName() << ") début destructeur";

    delete this->_xTopYRightZoomer;

    qDebug() << "AbstractDoubleAxisPlot (" << this->objectName() << ") fin destructeur";
}
