#include "ExtensibleEllipseItem.hpp"

ExtensibleEllipseItem::ExtensibleEllipseItem(QObject *parent) :
    QObject(parent)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable);
}

void ExtensibleEllipseItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (isSelected() && event->delta() != 0)
    {
        QRectF cur = rect();

        int sign = (event->delta() > 0)  ? 1 : -1;
        cur.setWidth(cur.width() + sign * 0.00005);
        cur.setHeight(cur.height() + sign * 0.00005);

        if (cur.width() >= 0.0001)
        {
            setRect(cur);
            event->accept();
            emit change();
        }
        else
        {
            event->ignore();
        }

    }
    else
    {
        event->ignore();
    }
}

void ExtensibleEllipseItem::paint(QPainter *painter,
                                  const QStyleOptionGraphicsItem *option,
                                  QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QColor color = isSelected() ? brush().color().lighter(125) : brush().color();

    painter->setPen(color.darker());
    painter->setBrush(QBrush(color));
    painter->drawEllipse(rect());
}
