#ifndef __DATAPOINT_HPP__
#define __DATAPOINT_HPP__

#include <QTime>

class DataPoint
{
    public:

        DataPoint(qreal x, qreal y, const QTime& time, int index = 0);

        // Getter
        int index(void) const;
        qreal x(void) const;
        qreal y(void) const;
        QTime timestamp(void) const;

        // Setter
        void setIndex(int index);

    protected:

        int   _index;
        qreal _x;
        qreal _y;
        QTime _time;
};

#endif /* __DATAPOINT_HPP__ */
