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
    signals:

        void pointSelected(float absciss, const QVariant& idTrack);
        void intervalSelected(float firstAbsciss, float secondAbsciss,
                              const QVariant& idTrack);

    public slots:

        void showCurves(bool visible);
        void showPoints(bool visible);
        void clearPlotSelection(void);
        void clearCurves(void);
        void highlightPoints(float timeValue, const QVariant& trackId);
        void highlightSector(float t1, float t2, const QVariant& trackId);

        /* Ajouter les méthodes pour afficher les labels
         * Pas oublier de créer un label lorsqu'on ajoute une curve et de les
         * supprimer lorsqu'on supprime les curves
         */

        void handleSelection(void);
        void lockSelectionAbility(void);
        void unlockSelectionAbility(void);

    protected:

        int selectionLocked;
        bool pointsVisible;
        bool curvesVisible;
        QList<PlotCurve*> curves;
        QList<QLabel*> pointPositionsLabels;
        QGraphicsItemGroup* selectedGroup;
};

#endif /* __PLOTSCENE_HPP__ */
