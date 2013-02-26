#include "AnimateSectorItem.hpp"

AnimateSectorItem::AnimateSectorItem(QObject *parent) :
    QObject(parent)
{
}

void AnimateSectorItem::launchAnimation(void)
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(10000);
    animation->setEasingCurve(QEasingCurve::OutElastic);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
