#ifndef __RACE_HPP__
#define __RACE_HPP__

#include <QtGui>

class Race
{
    public:

        Race(const QString& competition,
             const QDate& date = QDate::currentDate());

        int id(void) const;
        QString competition(void) const;
        QDate date(void) const;
        qreal wheelPerimeter(void) const;

        void setId(int id);
        void setDate(const QDate& date);
        void setWheelPerimeter(qreal peri);


        void addLap(const QTime& start, const QTime& end);
        int numLap(const QTime& t);
        QPair<QTime, QTime> lap(int ind);

        void display(void) const;

    protected:

        int _id;
        QString _competition;
        QDate _date;
        QLinkedList< QPair<QTime, QTime> > _laps;
        int _lastReturned;
        qreal _wheelPerimeter;
};

#endif /* __RACE_HPP__ */
