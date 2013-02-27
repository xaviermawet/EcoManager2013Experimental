#ifndef __ZONE_HPP__
#define __ZONE_HPP__

#include "DataPoint.hpp"
#include <QtGui>

class Zone
{
    public:

        Zone(void);
        virtual ~Zone(void);

        void add(DataPoint* dp);
        bool contains(DataPoint* dp) const;
        qreal dispersion(void) const;
        void timeGap(int num, QTime& start, QTime& end) const;
        int interval(int num, qreal width, qreal margin);
        QTime lastPoint(void) const;
        int gapCount(void) const;

        void display(void) const;

    protected:

        void expanse(qreal width);

        QList<int> diffs;
        QList<DataPoint*> points;
        qreal disp; // remplacer par dispersion
        int sumDiff;
};

#endif /* __ZONE_HPP__ */
