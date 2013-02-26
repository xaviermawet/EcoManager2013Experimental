#ifndef __PATHBUILDER_HPP__
#define __PATHBUILDER_HPP__

#include <QtGui>

class PathBuilder
{
    public:

        PathBuilder(const QPointF& p1, const QPointF& p2, qreal width);

        void append(const QPointF& newPoint);

        QPolygonF exBound(void) const;
        QPolygonF inBound(void) const;

    protected:

        QLineF _lastExLine;
        QLineF _lastInLine;
        QPointF _lastPoint;
        qreal _width;
        int _count;

        QVector<QPointF> _exPol;
        QVector<QPointF> _inPol;
};

#endif /* __PATHBUILDER_HPP__ */
