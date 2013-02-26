#include "TrackItem.hpp"

TrackItem::TrackItem(int id, QGraphicsItem *parent) :
    QGraphicsItem(parent), _internalId(id)
{
}

// insertion d'une coordonnée dans la liste chainée suivant le timestamp
void TrackItem::insertCoordinate(CoordinateItem* coord, float timestamp)
{
    QMutableLinkedListIterator< QPair<float, CoordinateItem*> > it(this->_timeTrack);
    bool inserted(false);

    while (!inserted && it.hasNext())
    {
        float curTime = it.next().first;

        if (curTime > timestamp)
            inserted = true;
    }

    it.previous();
    it.insert(QPair<float, CoordinateItem*>(timestamp, coord));
    coord->setParentItem(this);
}

QRectF TrackItem::boundingRect(void) const
{
    return this->childrenBoundingRect();
}

void TrackItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}

void TrackItem::setAcceptHoverEvents(bool enabled)
{
    QLinkedListIterator< QPair<float, CoordinateItem*> > it(this->_timeTrack);

    while(it.hasNext())
        it.next().second->setAcceptHoverEvents(enabled);

    //QGraphicsItem::setAcceptHoverEvents(enabled);
}

QVariant TrackItem::id(void) const
{
    return this->_internalId;
}

void TrackItem::setId(QVariant id)
{
    this->_internalId = id;
}

CoordinateItem* TrackItem::nearestCoord(float time)
{
    QLinkedListIterator< QPair<float, CoordinateItem*> > it(this->_timeTrack);
    bool found(false);
    QPair<float, CoordinateItem*> rElem;

    while (it.hasNext() && !found)
    {
        rElem = it.next();
        if (rElem.first > time)
            found = true;
    }

    if (found)
        return rElem.second;
    return NULL;
}

AnimateSectorItem* TrackItem::sectorOn(float t1, float t2)
{
    QLinkedListIterator< QPair<float, CoordinateItem*> > it(this->_timeTrack);
    QPair<float, CoordinateItem*> elem;
    AnimateSectorItem* sect(NULL);
    bool firstFound(false);
    bool secondFound(false);
    int index(0);
    float lowerBound = qMin(t1, t2);
    float upperBound = qMax(t1, t2);

    while (it.hasNext() && (!firstFound || !secondFound))
    {
        elem = it.next();

        if (!firstFound && elem.first > lowerBound)
        {
            firstFound = true;
            sect = new AnimateSectorItem;
        }

        if (firstFound)
        {
            IndexedPosition pos;
            pos.setX(elem.second->x());
            pos.setY(elem.second->y());
            pos.setIndex(index);
            index++;
            sect->append(pos);

            if (elem.first > upperBound)
            {
                secondFound = true;
            }
        }

    }

    return sect;
}

float TrackItem::getAssociateTime(const CoordinateItem* coord) const
{
    QLinkedListIterator< QPair<float, CoordinateItem*> > it(this->_timeTrack);
    bool found(false);
    QPair<float, CoordinateItem*> searchingTimeTrackItem;

    while (!found && it.hasNext())
    {
        searchingTimeTrackItem = it.next();

        if (searchingTimeTrackItem.second == coord)
            found = true;
    }

    return found ? searchingTimeTrackItem.first : -1;
}
