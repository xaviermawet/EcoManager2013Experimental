#ifndef __EXTENSIBLEELLIPSEITEM_HPP__
#define __EXTENSIBLEELLIPSEITEM_HPP__

#include <QtGui>

class ExtensibleEllipseItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    public:

        explicit ExtensibleEllipseItem(QObject* parent = 0);

    signals:

        void change(void);

    protected:

        virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
        virtual void paint(QPainter *painter,
                           const QStyleOptionGraphicsItem* option,
                           QWidget *widget);

        bool selecting;
};

#endif /* __EXTENSIBLEELLIPSEITEM_HPP__ */
