#ifndef __DOUBLEXAXISPLOT_HPP__
#define __DOUBLEXAXISPLOT_HPP__

#include "AbstractDoubleAxisPlot.hpp"
#include <qwt_plot_rescaler.h>

class DoubleXAxisPlot : public AbstractDoubleAxisPlot
{
    Q_OBJECT

    public:

        explicit DoubleXAxisPlot(
            QString const& title, double aspectRatio, QWidget* parent = NULL);
        explicit DoubleXAxisPlot(
            QwtText const& title, double aspectRatio, QWidget* parent = NULL);

        virtual ~DoubleXAxisPlot(void);

        virtual void zoom(QwtPlotItem const* item);

    protected:

        double           _ratio;
        QwtPlotRescaler* _xRescaler;
};

#endif /* __DOUBLEXAXISPLOT_HPP__ */
