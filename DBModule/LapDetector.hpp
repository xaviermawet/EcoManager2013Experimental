#ifndef __LAPDETECTOR_HPP__
#define __LAPDETECTOR_HPP__

#include "Zone.hpp"
#include "GeoCoordinate.hpp"
#include <QtGui>

class LapDetector
{
    public:

        LapDetector(const QVector<GeoCoordinate>* coords);
        ~LapDetector(void);

        QList< QPair<QTime, QTime> > laps(void) const;

    protected:

        inline bool seed(const QVector<GeoCoordinate>* coords);
        inline void process(void);
        inline void delimLaps(void);

        QVector< QVector<Zone*>* >* zones;
        QList<Zone*> timeline;
        QList< QPair<QTime, QTime> > gaps;
        qreal realAvg;
        qreal deviation;
};

#endif /* __LAPDETECTOR_HPP__ */
