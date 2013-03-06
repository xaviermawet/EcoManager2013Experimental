#ifndef __PLOTSCENE_HPP__
#define __PLOTSCENE_HPP__

#include "PlotCurve.hpp"
#include "../Common/ColorPicker.hpp"
#include <QtGui>

class PlotScene : public QGraphicsScene
{
    Q_OBJECT

    public:

        PlotScene(QObject* parent = 0);

        void addCurve(PlotCurve* curve);
        PlotCurve* addCurve(const QList<QPointF>& points,
                            const QVariant& curveId = QVariant());
        PlotCurve* addCurve(const QList<IndexedPosition>& points,
                            const QVariant& curveId = QVariant());

    public slots:

        void showCurves(bool visible);
        void showPoints(bool visible);
        void clearPlotSelection(void);
        void clearCurves(void);
        void highlightPoints(float timeValue, const QVariant& trackId);

    protected:

        bool pointsVisible;
        bool curvesVisible;
        QList<PlotCurve*> curves;
        QList<QLabel*> pointPositionsLabels;
        QGraphicsItemGroup* selectedGroup;
};

#endif /* __PLOTSCENE_HPP__ */
