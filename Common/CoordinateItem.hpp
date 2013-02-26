#ifndef __COORDINATEITEM_HPP__
#define __COORDINATEITEM_HPP__

#include <QtGui>

class CoordinateItem : public QGraphicsItem
{
    public:

        CoordinateItem(qreal x = 0, qreal y = 0, QGraphicsItem* parent = NULL);
        virtual ~CoordinateItem();

        virtual void paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget *widget);
        virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
        virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

        QRectF boundingRect(void) const;
        void setPen(const QPen& pen);
        void setHighlighted(bool enable);

        QVariant index(void) const;
        void setIndex(QVariant index);

        /* permettre l'utilisation de la fonction qgraphicsitem_cast
         * au lieu de bourriner avec un dynamic_cast<>
         */
        enum {Type = UserType + 1};
        int type(void) const { return Type; }

    protected:

        QPen _pen;
        bool _highlighted;
        QVariant _index;
};

#endif /* __COORDINATEITEM_HPP__ */
