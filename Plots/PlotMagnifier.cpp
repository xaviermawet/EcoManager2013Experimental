#include "PlotMagnifier.hpp"

PlotMagnifier::PlotMagnifier(QwtPlotCanvas *plotCanvas) :
    QwtPlotMagnifier(plotCanvas), _factors(1)
{
    // Nothing to do here ...
}

void PlotMagnifier::restoreScale(void)
{
    QwtPlotMagnifier::rescale(this->_factors);
    this->_factors = 1.0;
}

void PlotMagnifier::doRescale(double factor)
{
    qDebug() << "on rescale ...";
    QwtPlotMagnifier::rescale(factor);
}

void PlotMagnifier::rescale(double factor)
{
    QwtPlotMagnifier::rescale(factor);

    this->_factors *= (1 / factor);

    emit this->rescaled(factor);
}
