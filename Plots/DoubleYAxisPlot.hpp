#ifndef __DOUBLEYAXISPLOT_HPP__
#define __DOUBLEYAXISPLOT_HPP__

#include "AbstractDoubleAxisPlot.hpp"
#include <qwt_plot_rescaler.h>

class DoubleYAxisPlot : public AbstractDoubleAxisPlot
{
    Q_OBJECT

    public:

        explicit DoubleYAxisPlot(
            QString const& title, double aspectRatio, QWidget* parent = NULL);
        explicit DoubleYAxisPlot(
            QwtText const& title, double aspectRatio, QWidget* parent = NULL);

        virtual ~DoubleYAxisPlot(void);

        virtual void zoom(QwtPlotItem const* item);

    protected:

        double  _ratio;
        QwtPlotRescaler* _yRescaler;
};

#endif /* __DOUBLEYAXISPLOT_HPP__ */
