#include "TickItem.hpp"

TickItem::TickItem(bool animated, QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    if (animated)
        this->launchAnimation();
}

TickItem::~TickItem(void)
{
}

void TickItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* option,
                     QWidget* widget)
{
    Q_UNUSED (widget)

    QRadialGradient grad(QPoint(0, 0), 1);
    grad.setFocalPoint(0.5, 0.5);
    grad.setColorAt(0, Qt::white);

    if (!this->isSelected())
        grad.setColorAt(0.6, this->_color);
    else
        grad.setColorAt(0.6, this->_color);

    QBrush brush(grad);
    painter->setBrush(brush);
    painter->setPen(this->_color.darker());

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    if (lod > 6)
        painter->drawEllipse(-1, -1, 2, 2);
    else if (lod <= 6 && lod > 1)
        painter->drawEllipse(-2, -2, 4, 4);
    else
        painter->drawEllipse(-2.2, -2.2, 4.4, 4.4);
}

QRectF TickItem::boundingRect(void) const
{
    return QRectF(-2, -2, 4, 4);
}

QColor TickItem::color(void) const
{
    return this->_color;
}

void TickItem::setColor(QColor c)
{
    this->_color = c;
    this->update();
}

void TickItem::launchAnimation(void)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(5000);
    animation->setEasingCurve(QEasingCurve::OutElastic);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
