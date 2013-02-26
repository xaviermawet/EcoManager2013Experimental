#include "CoordinateItem.hpp"

CoordinateItem::CoordinateItem(qreal x, qreal y, QGraphicsItem *parent) :
    QGraphicsItem(parent), _highlighted(false)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    this->setFlag(QGraphicsItem::ItemIsSelectable);

    this->setPos(x, y);
    if (!(x == 0 && y == 0))
        this->setToolTip(QString("(%1,%2)").arg(this->x()).arg(this->y()));
}

CoordinateItem::~CoordinateItem()
{
}

void CoordinateItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* ,
                           QWidget*)
{
    painter->save();

    if (this->_highlighted)
    {
        painter->setPen(this->_pen.color().light());
        painter->fillRect(-3, -3, 6, 6, QBrush(this->_pen.color().light()));
    }
    else
    {
        if (this->isSelected())
        {
            QPen p(this->_pen.color().darker(130));
            painter->setPen(p);
        }
        else
        {
            painter->setPen(this->_pen);
        }

        painter->fillRect(-1, -1, 2, 2, QBrush(this->_pen.color()));
    }

    painter->restore();
}

void CoordinateItem::hoverEnterEvent(QGraphicsSceneHoverEvent* )
{
    this->_highlighted = true;
    this->update();
}

void CoordinateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    this->_highlighted = false;
    this->update();
}

QRectF CoordinateItem::boundingRect() const
{
    return QRectF(-3, -3, 6, 6);
}

void CoordinateItem::setPen(const QPen& pen)
{
    this->_pen = pen;
}

void CoordinateItem::setHighlighted(bool enable)
{
    this->_highlighted = enable;
}

QVariant CoordinateItem::index(void) const
{
    return _index;
}

void CoordinateItem::setIndex(QVariant index)
{
    this->_index = index;
}

