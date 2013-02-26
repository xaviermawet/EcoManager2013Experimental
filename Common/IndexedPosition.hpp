#ifndef __INDEXEDPOSITION_HPP__
#define __INDEXEDPOSITION_HPP__

#include <QPointF>

class IndexedPosition : public QPointF
{
    public:

        IndexedPosition(void);
        IndexedPosition(qreal x, qreal y, float index = 0);
        IndexedPosition(const QPointF& position, float index = 0);
        virtual ~IndexedPosition(void);

        float index(void) const;
        void  setIndex(float index);

    protected:

        float _index;
};

#endif /* __INDEXEDPOSITION_HPP__ */
