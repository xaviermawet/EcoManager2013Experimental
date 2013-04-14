#ifndef __PLOTCURVE2_HPP__
#define __PLOTCURVE2_HPP__

#include <qwt_plot_curve.h>

class PlotCurve2 : public QwtPlotCurve
{
    public:

        explicit PlotCurve2(QString const& title, QPen const& pen);
        explicit PlotCurve2(QwtText const& title, QPen const& pen);
};

#endif /* __PLOTCURVE2_HPP__ */
