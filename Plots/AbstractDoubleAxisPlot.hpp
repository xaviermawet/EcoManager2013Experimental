#ifndef __ABSTRACTDOUBLEAXISPLOT_HPP__
#define __ABSTRACTDOUBLEAXISPLOT_HPP__

#include "Plot.hpp"
#include "Zoomer.hpp"

class AbstractDoubleAxisPlot : public Plot
{
    Q_OBJECT

    public:

        explicit AbstractDoubleAxisPlot(
            QString const& title, QWidget* parent = NULL);
        explicit AbstractDoubleAxisPlot(
            QwtText const& title, QWidget* parent = NULL);

        virtual ~AbstractDoubleAxisPlot(void);

        virtual void zoom(QwtPlotItem const* item) = 0;

    protected:

        Zoomer* _xTopYRightZoomer;
};

#endif /* __ABSTRACTDOUBLEAXISPLOT_HPP__ */
