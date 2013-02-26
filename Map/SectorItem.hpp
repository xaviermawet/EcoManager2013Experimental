#ifndef __SECTORITEM_HPP__
#define __SECTORITEM_HPP__

#include "../Common/IndexedPosition.hpp"
#include "CurvePathBuilder.hpp"
#include <QGraphicsPathItem>

class SectorItem : public QGraphicsPathItem
{
    public:

        explicit SectorItem(const QString& owningCompetition = QString(),
                            QGraphicsItem* parent = 0);
        virtual ~SectorItem(void);

        QPair<SectorItem*, SectorItem*> split(QLineF splittingLine);
        void append(const IndexedPosition& point,
                    const QLineF& dirVect = QLineF());
        void append(const QList<IndexedPosition>& points);
        QPair<IndexedPosition, IndexedPosition> boundaries(void) const;
        IndexedPosition first(void) const;
        IndexedPosition last(void) const;
        QList<IndexedPosition> points(void) const;
        QString competition(void) const;
        void setOwningCompetition(const QString& owningCompetition);

    private:

        void updatePath(void);
        void addPoint(const IndexedPosition& point); // Amélioration

    protected:

        QList<IndexedPosition> _mainLine;
        CurvePathBuilder* _pathBuilder;
        QString _competition;
};

#endif /* __SECTORITEM_HPP__ */
