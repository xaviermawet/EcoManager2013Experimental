#include "Race.hpp"

Race::Race(const QString& competition, const QDate& date) :
    _id(-1), _competition(competition), _date(date),
    _lastReturned(0), _wheelPerimeter(1)
{
}

int Race::id(void) const
{
    return this->_id;
}

QString Race::competition(void) const
{
    return this->_competition;
}

QDate Race::date(void) const
{
    return this->_date;
}

qreal Race::wheelPerimeter(void) const
{
    return this->_wheelPerimeter;
}

void Race::setId(int id)
{
    this->_id = id;
}

void Race::setDate(const QDate& date)
{
    this->_date = date;
}

void Race::setWheelPerimeter(qreal peri)
{
    this->_wheelPerimeter = peri;
}

void Race::addLap(const QTime &start, const QTime &end)
{
    bool inserted(false);
    QMutableLinkedListIterator< QPair<QTime, QTime> > it(_laps);

    while (it.hasNext() && !inserted)
    {
        QPair<QTime, QTime> val = it.peekNext();

        if (start < val.first && end <= val.first)
        {
            it.insert(QPair<QTime, QTime>(start, end));
            inserted = true;
        }

        it.next();
    }

    if (!inserted)
        this->_laps.append(QPair<QTime, QTime>(start, end));
}

int Race::numLap(const QTime& t)
{
    if (this->_laps.isEmpty())
        return -1;

    int size(_laps.size());
    int count(0);
    QLinkedList< QPair<QTime, QTime> >::const_iterator it =
            this->_laps.constBegin() + this->_lastReturned;

    while (count < size && !(t >= it->first && t <= it->second))
    {
        if (it == this->_laps.constEnd())
            it = this->_laps.constBegin();
        else
            it++;

        count++;
    }

    if (count != size)
    {
        this->_lastReturned = (this->_lastReturned + count) % size;
        return this->_lastReturned;
    }

    return -1;
}

QPair<QTime, QTime> Race::lap(int ind)
{
    QLinkedListIterator< QPair<QTime, QTime> > it(this->_laps);
    int i(0);

    while(it.hasNext() && i < ind)
    {
        i++;
        it.next();
    }

    if (i == ind)
        return it.next();
    else
        return QPair<QTime, QTime>();
}

void Race::display(void) const
{
    QLinkedListIterator< QPair<QTime, QTime> > it(this->_laps);

    while(it.hasNext())
    {
        QPair<QTime, QTime> val = it.next();
        qDebug() << val.first.toString() << "-" << val.second.toString();
    }
}
