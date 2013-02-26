#include "IndexedPosition.hpp"

IndexedPosition::IndexedPosition(void) :
    QPointF(), _index(0)
{
}

IndexedPosition::IndexedPosition(qreal x, qreal y, float index) :
    QPointF(x, y), _index(index)
{
}

IndexedPosition::IndexedPosition(const QPointF &position, float index) :
    QPointF(position), _index(index)
{
}

IndexedPosition::~IndexedPosition(void)
{
}

float IndexedPosition::index(void) const
{
    return this->_index;
}

void IndexedPosition::setIndex(float index)
{
    this->_index = index;
}
