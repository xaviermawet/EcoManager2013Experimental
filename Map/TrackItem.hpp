#ifndef __TRACKITEM_HPP__
#define __TRACKITEM_HPP__

#include "AnimateSectorItem.hpp"
#include "../Common/CoordinateItem.hpp"
#include "../Common/IndexedPosition.hpp"
#include <QtGui>

class TrackItem : public QGraphicsItem
{
    public:

        explicit TrackItem(int id = 0, QGraphicsItem* parent = 0);

        void insertCoordinate(CoordinateItem* coord, float timestamp);

        virtual QRectF boundingRect(void) const;
        virtual void paint(QPainter*,const QStyleOptionGraphicsItem*, QWidget*);
        virtual void setAcceptHoverEvents(bool enabled);

        QVariant id(void) const;
        void setId(QVariant id);

        CoordinateItem* nearestCoord(float time);
        AnimateSectorItem* sectorOn(float t1, float t2);

        float getAssociateTime(const CoordinateItem* coord) const;

        enum { Type = UserType + 3 };
        int type() const { return Type; }

    protected:

        QVariant _internalId;
        QLinkedList< QPair<float, CoordinateItem*> > _timeTrack;
};

#endif // TRACKITEM_HPP
