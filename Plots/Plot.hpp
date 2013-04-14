#ifndef __PLOT_HPP__
#define __PLOT_HPP__

#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_legend_item.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_magnifier.h>
#include "Zoomer.hpp"
#include "PlotMagnifier.hpp"

class Plot : public QwtPlot
{
    Q_OBJECT

    public:

        explicit Plot(QString const& title, QWidget* parent = NULL);
        explicit Plot(QwtText const& title, QWidget* parent = NULL);
        virtual ~Plot(void);

        void zoom(QRectF const& zoomRectF);
        virtual void zoom(QwtPlotItem const* item);

        // Getter
        bool isGridVisible(void) const;
        bool isCrossLineVisible(void) const;
        bool isLabelPositionVisible(void) const;
        QwtPlotGrid*   grid(void) const;
        QwtPlotMarker* crossLine(void) const;
        Zoomer* zoomer(void) const;

    signals:

        void legendRightClicked(QwtPlotItem const* item, QPoint const& pos);

    public slots:

        // Setter
        void setGridVisible(bool visible);
        void setCrossLineVisible(bool visible);
        void setLabelPositionVisible(bool visible);

    protected slots:

        // Personnal slots
        void updateCrossLinePosition(QPointF const& pos);
        void showLegendContextMenu(QPoint const& pos);

    protected:

        QwtLegend*     _legend;
        QwtPlotGrid*   _grid;
        QwtPlotMarker* _crossLine;
        QwtPlotPanner* _panner;
        Zoomer*        _xBottomYLeftZoomer;
        PlotMagnifier* _magnifier;

        bool           _labelWasVisible;
};

#endif /* __PLOT_HPP__ */
