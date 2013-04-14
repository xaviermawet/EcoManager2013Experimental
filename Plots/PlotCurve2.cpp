#include "PlotCurve2.hpp"

PlotCurve2::PlotCurve2(const QString &title, const QPen &pen) :
    PlotCurve2(QwtText(title), pen)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

PlotCurve2::PlotCurve2(const QwtText &title, const QPen &pen) :
    QwtPlotCurve(title)
{
    this->setPen(pen);
    this->setRenderHint(QwtPlotItem::RenderAntialiased);
    this->setItemAttribute(QwtPlotItem::Legend); // true default value
    //this->setLegendAttribute(LegendShowLine);
}
