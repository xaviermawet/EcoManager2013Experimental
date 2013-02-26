#ifndef __CURVEPATHBUILDER_HPP__
#define __CURVEPATHBUILDER_HPP__

#include <QtGui>

class CurvePathBuilder
{
    public:

        CurvePathBuilder(const QPointF& position1, const QPointF& position2,
                         qreal width = 2.0);

        QPainterPath inBound(void) const;
        QPainterPath exBound(void) const;

        void append(const QPointF& newPoint);

    protected:

        qreal _width;
        int _currentIndex;
        QPainterPath _inBound;
        QPainterPath _exBound;
        QPointF _lastPoint;
        QPointF _controlPoint;
};

#endif /* __CURVEPATHBUILDER_HPP__ */
