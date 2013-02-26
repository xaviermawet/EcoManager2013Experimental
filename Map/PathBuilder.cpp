#include "PathBuilder.hpp"

PathBuilder::PathBuilder(const QPointF& p1, const QPointF& p2, qreal width) :
    _lastPoint(p2), _width(width / 2), _count(0)
{
    QLineF firstLine(p1, p2);
    QLineF unitNormalVector = firstLine.normalVector().unitVector();

    this->_lastInLine = firstLine.translated(-this->_width * unitNormalVector.dx(),
                                             -this->_width * unitNormalVector.dy());
    this->_lastExLine = firstLine.translated(this->_width * unitNormalVector.dx(),
                                             this->_width * unitNormalVector.dy());

    this->_inPol.append(this->_lastInLine.p1());
    this->_exPol.append(this->_lastExLine.p1());
}

void PathBuilder::append(const QPointF& newPoint)
{
    if (this->_lastPoint == newPoint)
        return;

    QLineF lineToAdd(this->_lastPoint, newPoint);
    QLineF unitNormalVector = lineToAdd.normalVector().unitVector();

    qDebug() << unitNormalVector << unitNormalVector.dx() << unitNormalVector.dy();

    QLineF newInLine = lineToAdd.translated(-this->_width * unitNormalVector.dx(),
                                            -this->_width * unitNormalVector.dy());
    QLineF newExLine = lineToAdd.translated(this->_width * unitNormalVector.dx(),
                                            this->_width * unitNormalVector.dy());

    QPointF intersectionInPoint;
    int intersectionType = this->_lastInLine.intersect(newInLine,
                                                       &intersectionInPoint);

    if (intersectionType == QLineF::BoundedIntersection)
    {
        this->_lastInLine.setP2(intersectionInPoint);
        newInLine.setP1(intersectionInPoint);
        this->_inPol.append(this->_lastInLine.p2());
    }
    else if (intersectionType == QLineF::UnboundedIntersection)
    {
        QLineF gap(this->_lastPoint, intersectionInPoint);
        QLineF tmp(this->_lastInLine.p2(), this->_lastInLine.p1());

        if (gap.length() > this->_width * 1.1 && newInLine.angleTo(tmp) < 180)
        {
            gap.setLength(this->_width);
            this->_inPol.append(this->_lastInLine.p2());
            this->_inPol.append(gap.p2());
            this->_inPol.append(newInLine.p1());
        }
        else
        {
            this->_lastInLine.setP2(intersectionInPoint);
            newInLine.setP1(intersectionInPoint);
            this->_inPol.append(this->_lastInLine.p2());
        }
    }
    else
    {
        qWarning() << "not intersecting";
    }

    QPointF intersectionExPoint;
    intersectionType = this->_lastExLine.intersect(newExLine,
                                                   &intersectionExPoint);

    if (intersectionType == QLineF::BoundedIntersection)
    {
        this->_lastExLine.setP2(intersectionExPoint);
        newExLine.setP1(intersectionExPoint);
        this->_exPol.append(this->_lastExLine.p2());
    }
    else if (intersectionType == QLineF::UnboundedIntersection)
    {
        QLineF gap(this->_lastPoint, intersectionExPoint);
        QLineF tmp(this->_lastExLine.p2(), this->_lastExLine.p1());

        if (gap.length() > this->_width * 1.1  && newExLine.angleTo(tmp) > 180)
        {
            gap.setLength(this->_width);
            this->_exPol.append(this->_lastExLine.p2());
            this->_exPol.append(gap.p2());
            this->_exPol.append(newExLine.p2());
        }
        else
        {
            this->_lastExLine.setP2(intersectionExPoint);
            newExLine.setP1(intersectionExPoint);
            this->_exPol.append(this->_lastExLine.p2());
        }
    }

    this->_lastPoint = newPoint;
    this->_lastInLine = newInLine;
    this->_lastExLine = newExLine;
}

QPolygonF PathBuilder::exBound(void) const
{
    return this->_exPol;
}

QPolygonF PathBuilder::inBound(void) const
{
    return this->_inPol;
}
