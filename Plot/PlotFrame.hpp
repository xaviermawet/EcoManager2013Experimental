#ifndef __PLOTFRAME_HPP__
#define __PLOTFRAME_HPP__

#include "PlotView.hpp"
#include "HorizontalScale.hpp"
#include "VerticalScale.hpp"
#include "PlotCurve.hpp"
#include "../Common/IndexedPosition.hpp"
#include "../Map/AnimateSectorItem.hpp"
#include "../Common/ColorPicker.hpp"
#include "../Common/CoordinateItem.hpp"
#include <QtGui>

class PlotFrame : public QFrame
{
    Q_OBJECT

    public:

        explicit PlotFrame(QFrame *parent = 0);

        void addCurve(PlotCurve*);
        PlotCurve* addCurve(QList<QPointF>, QVariant curveId=QVariant());
        PlotCurve* addCurve(QList<IndexedPosition>, QVariant curveId=QVariant());
        void addVerticalAxis(VerticalScale*);
        void addHorizontalAxis(HorizontalScale*);
        QGraphicsScene* scene() const;

    signals:

        void positionChange(QPoint);
        void pointSelected(float absciss, QVariant idTrack);
        void intervalSelected(float firstAbsciss, float secondAbsciss, QVariant idTrack);
        void selectionChanged();
        void minVChanged(float);
        void maxVChanged(float);
        void minHChanged(float);
        void maxHChanged(float);
        void clear(void);

    public slots:

        void showCurves(bool);
        void showPoints(bool);
        void clearCurves(void);
        void clearPlotSelection(void);
        void highlightPoint(float absciss, QVariant idTrack);
        void highlightSector(float t1, float t2, QVariant trackId);

    protected slots:

        void adaptScales(const QRectF&);
        void handleSelection();
        void lockSelectionAbility();
        void unlockSelectionAbility();

    private:

        void createView();
        void createToolBar();

    protected:

        QGraphicsScene* _scene;
        PlotView* _mainview;
        QList<PlotCurve*> _curves; // Liste de tous les tracés (tous les tours)
        QGraphicsItemGroup* _selectedGroup;
        QVBoxLayout* _topScaleLayout;
        QVBoxLayout* _bottomScaleLayout;
        QHBoxLayout* _leftScaleLayout;
        QHBoxLayout* _rightScaleLayout;
        QToolBar* _toolbar;
        int _selectionLocked;
        bool _pointsVisible;
        bool _curveVisible;
};

#endif /* __PLOTFRAME_HPP__ */
