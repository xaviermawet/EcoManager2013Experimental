#include "PlotCurve.hpp"

PlotCurve::PlotCurve(QVariant id, QGraphicsItem *parent) :
    QGraphicsItem(parent), last(NULL), internalId(id)
{
}

PlotCurve::PlotCurve(const QList<QPointF> &p, QVariant id,
                     QGraphicsItem *parent) :
    QGraphicsItem(parent), last(NULL), internalId(id)
{
    foreach (const QPointF& point, p)
    {
        CoordinateItem* item = new CoordinateItem(point.x(), point.y(), this);
        item->setAcceptHoverEvents(true);
        this->addToCurve(item);
    }
}

PlotCurve::PlotCurve(const QList<IndexedPosition> &p, QVariant id,
                     QGraphicsItem *parent) :
    QGraphicsItem(parent), last(NULL), internalId(id)
{
    foreach (const IndexedPosition& ip, p)
    {
        CoordinateItem* item = new CoordinateItem(ip.x(), ip.y(), this);
        item->setIndex(ip.index());
        item->setAcceptHoverEvents(true);
        this->addToCurve(item);
    }
}

void PlotCurve::addToCurve(QGraphicsItem *item)
{
    /* S'il y a déjà un point qui compose le tracé, on ajoute une ligne de ce
     * dernier point, jusqu'au coordonnées du item (qui est en réalité
     * un CoordinateItem ) */
    if (last != NULL)
    {
        QGraphicsLineItem* line = new QGraphicsLineItem(last->x() , last->y(), item->x(), item->y(), this);
        line->setPen(pen);
        curve << line; // Ajout du segment de ligne au tracé
    }

    /* Ajout du point (CoordinateItem) à la liste des points qui composent
     * le tracé + renseigné comme étant le dernier point ajouté */
    CoordinateItem* citem = qgraphicsitem_cast<CoordinateItem*>(item);
    if (citem != NULL)
    {
        QPen p(pen.color().dark());
        p.setWidth(5);
        citem->setPen(p);
        citem->setAcceptHoverEvents(true);
        points << citem;
        last = item;
    }
}

void PlotCurve::setCurveVisible(bool visible)
{
    curveVisible = visible;

    foreach (QGraphicsLineItem* item, curve)
        item->setVisible(visible);
}

void PlotCurve::setPointsVisible(bool visible)
{
    pointsVisible = visible;

    foreach (QGraphicsItem* item, points)
        item->setVisible(visible);
}

void PlotCurve::setPen(const QPen& p)
{
    pen = p;

    foreach (QGraphicsItem* item, curve)
    {
        QGraphicsLineItem* lineItem =  (QGraphicsLineItem*) item;
        lineItem->setPen(p);
    }

    foreach (QGraphicsItem* item, points)
    {
        CoordinateItem* citem = qgraphicsitem_cast<CoordinateItem*> (item);

        if (citem != NULL)
            citem->setPen(pen);
    }
}

QVariant PlotCurve::id(void) const
{
    return internalId;
}

QRectF PlotCurve::boundingRect() const
{
    return childrenBoundingRect();
}

void PlotCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

QList<CoordinateItem*> PlotCurve::nearestCoordinateitems(float timeValue) const
{
    int seconde = qRound(timeValue);
    QList<CoordinateItem*> coordinatesAtFixedTime;
    CoordinateItem* currentItem = NULL;

    for(int i(0); i < this->points.count(); i++)
    {
        currentItem = qgraphicsitem_cast<CoordinateItem*>(this->points.at(i));

        if (currentItem == NULL)
            continue;

        /* Vérifie si la valeur absolue de l'heure à laquelle la coordonnée
         * du graphique correspond à la valeur absolue de l'heure à laquelle
         * la coordonnée GPS sélectionnée dans la vue mapping à été prise */
        if (qRound(currentItem->index().toFloat()) == seconde)
            coordinatesAtFixedTime.append(currentItem);
    }

    return coordinatesAtFixedTime;
}

QList<CoordinateItem*> PlotCurve::onSector(float minTimeValue, float maxTimeValue) const
{
    int lowerBound = qRound(minTimeValue);
    int upperBound = qRound(maxTimeValue);
    QList<CoordinateItem*> items;
    CoordinateItem* currentItem = NULL;
    int currentItemTime;

    for (int i(0); i < this->points.count(); i++)
    {
        currentItem = qgraphicsitem_cast<CoordinateItem*>(this->points.at(i));

        if (currentItem == NULL)
            continue;

        currentItemTime = qRound(currentItem->index().toFloat());
        if(currentItemTime >= lowerBound && currentItemTime <= upperBound)
            items.append(currentItem);
    }

    return items;
}

AnimateSectorItem* PlotCurve::sectorOn(float timeValue) const
{
    int seconde = qRound(timeValue);
    CoordinateItem* currentItem = NULL;
    int index(0);
    AnimateSectorItem* sect = NULL;

    for(int i(0); i < this->points.count(); i++)
    {
        currentItem = qgraphicsitem_cast<CoordinateItem*>(this->points.at(i));

        if (currentItem == NULL)
            continue;

        /* Vérifie si la valeur absolue de l'heure à laquelle la coordonnée
         * du graphique correspond à la valeur absolue de l'heure à laquelle
         * la coordonnée GPS sélectionnée dans la vue mapping à été prise */
        if (qRound(currentItem->index().toFloat()) == seconde)
        {
            IndexedPosition pos;
            pos.setX(currentItem->x());
            pos.setY(currentItem->y());
            pos.setIndex(index++);

            if(sect == NULL)
                sect = new AnimateSectorItem;

            sect->append(pos);
        }
    }

    return sect;
}

AnimateSectorItem* PlotCurve::sectorOn(float minTimeValue, float maxTimeValue) const
{
    int lowerBound = qRound(minTimeValue);
    int upperBound = qRound(maxTimeValue);
    CoordinateItem* currentItem = NULL;
    int currentItemTime;
    int index(0);
    AnimateSectorItem* sect = NULL;

    for (int i(0); i < this->points.count(); i++)
    {
        currentItem = qgraphicsitem_cast<CoordinateItem*>(this->points.at(i));

        if (currentItem == NULL)
            continue;

        currentItemTime = qRound(currentItem->index().toFloat());
        if(currentItemTime >= lowerBound && currentItemTime <= upperBound)
        {
            IndexedPosition pos;
            pos.setX(currentItem->x());
            pos.setY(currentItem->y());
            pos.setIndex(index++);

            if(sect == NULL)
                sect = new AnimateSectorItem;

            sect->append(pos);
        }
    }

    return sect;
}
