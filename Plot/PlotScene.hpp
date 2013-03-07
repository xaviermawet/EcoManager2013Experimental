#ifndef __PLOTSCENE_HPP__
#define __PLOTSCENE_HPP__

#include "PlotCurve.hpp"
#include "../Common/ColorPicker.hpp"
#include <QtGui>

class PlotScene : public QGraphicsScene
{
    Q_OBJECT

    public:

        PlotScene(QWidget *widgetParent, QObject* parent = 0);

        void addCurve(PlotCurve* curve);
        PlotCurve* addCurve(const QList<QPointF>& points,
                            const QVariant& curveId = QVariant());
        PlotCurve* addCurve(const QList<IndexedPosition>& points,
                            const QVariant& curveId = QVariant());

        bool curvesAreVisible(void) const;
        bool pointsAreVisible(void) const;
        bool curveLabelsAreVisible(void) const;

    signals:

        void pointSelected(float absciss, const QVariant& idTrack);
        void intervalSelected(float firstAbsciss, float secondAbsciss,
                              const QVariant& idTrack);

    public slots:

        void setCurvesVisible(bool visible);
        void setPointsVisible(bool visible);
        void setCurveLabelsVisible(bool visible);
        void clearPlotSelection(void);
        void clearCurves(void);
        void highlightPoints(float timeValue, const QVariant& trackId);
        void highlightSector(float t1, float t2, const QVariant& trackId);
        void displayLabels(const QPointF& mousePos, const QPointF& scenePos);

        /* Ajouter les méthodes pour afficher les labels
         * Pas oublier de créer un label lorsqu'on ajoute une curve et de les
         * supprimer lorsqu'on supprime les curves
         */

        void handleSelection(void);
        void lockSelectionAbility(void);
        void unlockSelectionAbility(void);

    protected:

        int  selectionLocked;
        bool pointsVisible;
        bool curvesVisible;
        bool curveLabelsVisible;

        QList<PlotCurve*> curves;
        QList<QLabel*> curveLabels;
        QGraphicsItemGroup* selectedGroup;

        QWidget* widgetParent;
};

#endif /* __PLOTSCENE_HPP__ */
