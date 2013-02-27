#include "DataPoint.hpp"

DataPoint::DataPoint(qreal x, qreal y, const QTime &time, int index) :
    _index(index), _x(x), _y(y), _time(time)
{
}

int DataPoint::index(void) const
{
    return this->_index;
}

qreal DataPoint::x(void) const
{
    return this->_x;
}

qreal DataPoint::y(void) const
{
    return this->_y;
}

QTime DataPoint::timestamp(void) const
{
    return this->_time;
}

void DataPoint::setIndex(int index)
{
    this->_index = index;
}
