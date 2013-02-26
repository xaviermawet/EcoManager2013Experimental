#include "SectorItem.hpp"

SectorItem::SectorItem(const QString& owningCompetition, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), _pathBuilder(NULL),
      _competition(owningCompetition)
{
}

SectorItem::~SectorItem(void)
{
    delete this->_pathBuilder;
}

QPair<SectorItem *, SectorItem *> SectorItem::split(QLineF splittingLine)
{
    int currentIndex(1);
    int count(this->_mainLine.count());
    bool match(false);
    QPointF pointIntersection;
    QPair<SectorItem*, SectorItem*> subSectors;

    while (currentIndex < count && !match)
    {
        QLineF segment(this->_mainLine.at(currentIndex - 1),
                       this->_mainLine.at(currentIndex));

        if (splittingLine.intersect(segment, &pointIntersection)
                == QLineF::BoundedIntersection)
        {
            SectorItem* first = new SectorItem(this->_competition);
            SectorItem* second = new SectorItem(this->_competition);

            IndexedPosition indexedIntersection(pointIntersection);
            float intersectionIndex = this->_mainLine[currentIndex].index();
            indexedIntersection.setIndex(intersectionIndex);

            first->append(this->_mainLine.mid(0, currentIndex));
            first->append(indexedIntersection, splittingLine);

            second->append(indexedIntersection, splittingLine);
            second->append(this->_mainLine.mid(currentIndex));

            subSectors.first = first;
            subSectors.second = second;
            match = true;
        }

        currentIndex++;
    }

    return subSectors;
}

void SectorItem::append(const IndexedPosition& point, const QLineF& dirVect)
{
    Q_UNUSED(dirVect);

    /*
    this->_mainLines << point;

    if (this->_mainLines.count() == 2)
    {
        this->_pathBuilder = new CurvePathBuilder(this->_mainLines.at(0),
                                                  this->_mainLines.at(1));
    }
    else if (this->_mainLines.count() > 2)
    {
        this->_pathBuilder->append(point);
        this->updatePath();
    }
    */

    this->addPoint(point);
    this->updatePath();
}

void SectorItem::append(const QList<IndexedPosition>& points)
{
    /*
    int count(points.count());
    int i(0);

    if (this->_pathBuilder == NULL && this->_mainLines.count() + count >= 2)
    {
        for(; this->_mainLines.count() < 2; i++)
            this->_mainLines << points.at(i);

        this->_pathBuilder = new CurvePathBuilder(this->_mainLines.at(0),
                                                  this->_mainLines.at(1));
    }

    if (this->_mainLines.count() < 2)
        return;

    for(; i < count; i++)
    {
        this->_mainLines << points.at(i);
        this->_pathBuilder->append(points.at(i));
    }

    this->updatePath();
    */

    for(int i(0); i < points.count(); ++i)
        this->addPoint(points.at(i));

    this->updatePath();
}

QPair<IndexedPosition, IndexedPosition> SectorItem::boundaries(void) const
{
    return QPair<IndexedPosition, IndexedPosition>(this->_mainLine.first(),
                                                   this->_mainLine.last());
}

IndexedPosition SectorItem::first(void) const
{
    return this->_mainLine.first();
}

IndexedPosition SectorItem::last(void) const
{
    return this->_mainLine.last();
}

QList<IndexedPosition> SectorItem::points(void) const
{
    return this->_mainLine;
}

QString SectorItem::competition(void) const
{
    return this->_competition;
}

void SectorItem::setOwningCompetition(const QString& owningCompetition)
{
    this->_competition = owningCompetition;
}

void SectorItem::updatePath()
{
    if (this->_pathBuilder == NULL) return;

    QPainterPath newPath;
    newPath.addPath(this->_pathBuilder->inBound());

    QPainterPath exBound = this->_pathBuilder->exBound().toReversed();
    newPath.connectPath(exBound);
    this->setPath(newPath);
}

void SectorItem::addPoint(const IndexedPosition &point)
{
    this->_mainLine << point;

    if (this->_mainLine.count() == 2)
        this->_pathBuilder = new CurvePathBuilder(this->_mainLine.at(0),
                                                  this->_mainLine.at(1));
    else if (this->_mainLine.count() > 2)
        this->_pathBuilder->append(point);
}
