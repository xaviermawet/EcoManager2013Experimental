#include "Zoomer.hpp"

Zoomer::Zoomer(QwtPlotCanvas* canvas, bool doReplot) :
    Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas, doReplot)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

Zoomer::Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas, bool doReplot) :
    QwtPlotZoomer(xAxis, yAxis, canvas, doReplot)
{
    this->setTrackerMode( QwtPicker::AlwaysOff );
    this->setRubberBand( QwtPicker::NoRubberBand );

    // RightButton: zoom out by 1
    // Ctrl+RightButton: zoom out to full size

    this->setMousePattern( QwtEventPattern::MouseSelect2,
                           Qt::RightButton, Qt::ControlModifier );
    this->setMousePattern( QwtEventPattern::MouseSelect3,
                           Qt::RightButton );
}

QwtText Zoomer::trackerTextF(const QPointF& pos) const
{
    emit this->mousePosChanged(pos);

    return QwtPlotZoomer::trackerTextF(pos);
}
