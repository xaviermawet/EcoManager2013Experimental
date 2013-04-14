#ifndef __ZOOMER_HPP__
#define __ZOOMER_HPP__

#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>

class Zoomer: public QwtPlotZoomer
{
    Q_OBJECT

    public:

        explicit Zoomer(QwtPlotCanvas* canvas, bool doReplot = true);
        explicit Zoomer(int xAxis, int yAxis, QwtPlotCanvas* canvas,
                        bool doReplot = true);

        virtual QwtText trackerTextF(const QPointF& pos) const;

    signals:

        void mousePosChanged(const QPointF& pos) const;
};

#endif /* __ZOOMER_HPP__ */
