#ifndef __PLOTMAGNIFIER_HPP__
#define __PLOTMAGNIFIER_HPP__

#include <qwt_plot_magnifier.h>
#include <QDebug>

class PlotMagnifier : public QwtPlotMagnifier
{
    Q_OBJECT

    public:

        explicit PlotMagnifier(QwtPlotCanvas* plotCanvas);

        void restoreScale(void);

    signals:

        void rescaled(double factor);

    public slots:

        virtual void doRescale(double factor);

    protected:

        virtual void rescale(double factor);

    protected:

        double _factors;
};

#endif /* __PLOTMAGNIFIER_HPP__ */
