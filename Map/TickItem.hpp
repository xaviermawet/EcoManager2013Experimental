#ifndef __TICKITEM_HPP__
#define __TICKITEM_HPP__

#include <QtGui>

class TickItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)

    public:

        explicit TickItem(bool animated = true, QGraphicsItem* parent = 0);
        virtual ~TickItem(void);

        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget);
        QRectF boundingRect(void) const;

        QColor color(void) const;
        void setColor(QColor c);

        enum {Type = UserType + 2};
        int type(void) const { return Type; }

    public slots:

        void launchAnimation(void);

    protected:

        QColor _color;

};

#endif /* __TICKITEM_HPP__ */
