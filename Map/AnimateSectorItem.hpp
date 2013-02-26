#ifndef __ANIMATESECTORITEM_HPP__
#define __ANIMATESECTORITEM_HPP__

#include "SectorItem.hpp"
#include <QObject>

class AnimateSectorItem : public QObject, public SectorItem
{
    Q_OBJECT

    //on expose ici le getter/setter expose par QGraphicsItem
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

    public:

        explicit AnimateSectorItem(QObject* parent = 0);
        void launchAnimation(void);
};

#endif /* __ANIMATESECTORITEM_HPP__ */
