#include "CurvePathBuilder.hpp"

CurvePathBuilder::CurvePathBuilder(const QPointF& position1,
                                   const QPointF& position2, qreal width) :
    _width(width / 2), _currentIndex(0)
{
    QLineF firstLine(position1, position2);
    QLineF unitNormalVector = firstLine.normalVector().unitVector();

    unitNormalVector.setLength(this->_width);
    this->_inBound.moveTo(unitNormalVector.p2());

    unitNormalVector.setLength(-this->_width);
    this->_exBound.moveTo(unitNormalVector.p2());

    this->append(position2);
}

QPainterPath CurvePathBuilder::inBound(void) const
{
    QPainterPath rPath = this->_inBound;

    if (this->_currentIndex == 1)
    {
        QLineF lastLine(this->_lastPoint, this->_controlPoint);
        QLineF unitNormalVector = lastLine.normalVector().unitVector();

        lastLine.translate(unitNormalVector.dx() * this->_width,
                           unitNormalVector.dy() * this->_width);

        rPath.quadTo(lastLine.p1(), lastLine.p2());
    }

    return rPath;
}

QPainterPath CurvePathBuilder::exBound(void) const
{
    QPainterPath rPath = this->_exBound;

    if (this->_currentIndex == 1)
    {
        QLineF lastLine(this->_lastPoint, this->_controlPoint);
        QLineF unitNormalVector = lastLine.normalVector().unitVector();

        lastLine.translate(unitNormalVector.dx() * -this->_width,
                           unitNormalVector.dy() * -this->_width);

        rPath.quadTo(lastLine.p1(), lastLine.p2());
    }

    return rPath;
}

void CurvePathBuilder::append(const QPointF& newPoint)
{
    if (this->_currentIndex < 1)
    {
        this->_lastPoint = this->_controlPoint;
        this->_controlPoint = newPoint;
        this->_currentIndex++;
    }
    else
    {
        QLineF lastLine(this->_controlPoint, newPoint);
        QLineF unitNormalVector = lastLine.normalVector().unitVector();

        QLineF inLine = lastLine.translated(unitNormalVector.dx() * this->_width,
                                            unitNormalVector.dy() * this->_width);
        QLineF exLine = lastLine.translated(unitNormalVector.dx() * -this->_width,
                                            unitNormalVector.dy() * -this->_width);

        this->_inBound.quadTo(inLine.p1(), inLine.p2());
        this->_exBound.quadTo(exLine.p1(), exLine.p2());
        this->_currentIndex = 0;
    }
}
