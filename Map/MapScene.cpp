#include "MapScene.hpp"

MapScene::MapScene(qreal ratio, QObject* parent) :
    QGraphicsScene(parent), _amplificationRatio(ratio),
    _trackAcceptHoverEvents(false), _selectedGroup(NULL)
{
}

MapScene::~MapScene(void)
{
    delete this->_selectedGroup;
}

void MapScene::addSector(const QVector<IndexedPosition>& points,
                         const QString& targetCompetition)
{
    // Initialisation du secteur
    SectorItem* newSector = new SectorItem(targetCompetition);

    // Ajout de tous les points dans le secteur
    foreach (const IndexedPosition& p, points)
        newSector->append(IndexedPosition(p.x() * this->_amplificationRatio,
                                          p.y() * -this->_amplificationRatio,
                                          p.index()));

    // Colorisation du secteur
    ColorPicker picker(6);
    QColor sectorColor(picker.color(this->_sectors.count(), 150));
    newSector->setBrush(QBrush(sectorColor));

    QPen pen(picker.dark(this->_sectors.count()));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidthF(1.5);
    pen.setCosmetic(true);
    newSector->setPen(pen);
    newSector->setZValue(-1);

    this->_sectors << newSector;
    this->addItem(newSector);
}

bool MapScene::hasSectors(void) const
{
    return !this->_sectors.isEmpty();
}

void MapScene::mergeSector(int fstNum, int sndNum)
{
    if (this->_sectors.isEmpty())
        return;

    if (fstNum == -1)
    {
        fstNum = 0;
        if (this->_sectors.count() > 1)
            sndNum = 1;
    }
    else if (sndNum == this->_sectors.count())
    {
        fstNum--;
        sndNum--;
    }
    else if (fstNum > this->_sectors.count() - 1)
    {
        return;
    }

    // modify upper boundary of the first sector and delete the second one
    SectorItem* fst = this->_sectors.at(fstNum);
    SectorItem* snd = this->_sectors.at(sndNum);

    if (fst != snd)
    {
        fst->append(snd->points().mid(1));
        emit sectorUpdated(fst->competition(), fstNum, fst->first(), fst->last());
    }

    this->_sectors.removeAt(sndNum);
    this->removeItem(snd);
    emit sectorRemoved(snd->competition(), sndNum);

    this->reorderSectorColors();
}

void MapScene::addTrack(const QVector<QPointF>& points)
{
    if (points.isEmpty())
        return;

    QPainterPath route;

    for (int i(0); i < points.count(); i++)
    {
        qreal sx = points[i].x() * this->_amplificationRatio;
        qreal sy = (points[i].y() * (-this->_amplificationRatio));

        CoordinateItem* ci = new CoordinateItem;
        ci->setPos(sx, sy);
        ci->setData(0, i);
        this->addItem(ci);

        if (i == 0)
            route.moveTo(sx, sy);
        else
            route.lineTo(sx, sy);
    }

    this->addPath(route, QPen(Qt::white));
    this->setSceneRect(this->itemsBoundingRect());
}

void MapScene::addTrack(const QVector<QPointF>& points,
                        const QVector<float>& metaData, QVariant idTrack)
{
    if (points.isEmpty())
        return;

    QPainterPath route;
    TrackItem* track = new TrackItem;
    track->setId(idTrack);

    // Add each CoordinateItems which constitute the track
    for (int i(0); i < points.count(); i++)
    {
        qreal sx = points[i].x() * this->_amplificationRatio;
        qreal sy = points[i].y() * -this->_amplificationRatio;

        CoordinateItem* ci = new CoordinateItem;
        ci->setPos(sx, sy);
        ci->setAcceptHoverEvents(this->_trackAcceptHoverEvents);
        track->insertCoordinate(ci, metaData.value(i));

        if (i == 0)
            route.moveTo(sx, sy);
        else
            route.lineTo(sx, sy);
    }

    this->_tracks << track;
    this->addItem(track);
    QGraphicsItem* path = this->addPath(route, QPen(Qt::white));
    path->setParentItem(track);
    this->setSceneRect(this->itemsBoundingRect());
}

void MapScene::fixSymbol(float timeValue, QColor color, QVariant trackId)
{
    //FIXME !!
    TrackItem* targetTrack(NULL);
    int idTrack(-1);

    qDebug() << "fixSymbol :" << this->_tracks.count() << this->_tracks;

    for (int i(0); idTrack == -1 && i < this->_tracks.count(); i++)
    {
        TrackItem* tr = this->_tracks[i];

        if (tr->id() == trackId)
        {
            idTrack = i;
            targetTrack = tr;
        }
    }

    if (idTrack != -1)
    {
        qDebug() << "track found";

        CoordinateItem* nearestCoord = targetTrack->nearestCoord(timeValue);

        if (nearestCoord != NULL)
        {
            qDebug() << "nearest coord found";

            TickItem* tick = new TickItem(false);
            tick->setPos(nearestCoord->pos());
            tick->setZValue(10);
            tick->setColor(color);

            QGraphicsItemGroup* symbolGroup;

            if (!this->_symbols.contains(idTrack))
            {
                symbolGroup = new QGraphicsItemGroup;
                this->addItem(symbolGroup);
                this->_symbols[idTrack] = symbolGroup;
            }
            else
            {
                symbolGroup = this->_symbols[idTrack];
            }

            symbolGroup->addToGroup(tick);
        }
    }
}

void MapScene::removeSymbol(QVariant trackId)
{
    int idTrack(-1);

    for (int i(0); idTrack == -1 && i < this->_tracks.count(); i++)
    {
        TrackItem* tr = this->_tracks[i];
        if (tr->id() == trackId)
            idTrack = i;
    }

    if (this->_symbols.contains(idTrack))
    {
        QGraphicsItemGroup* group = this->_symbols.value(idTrack);
        this->removeItem(group);
        this->_symbols.remove(idTrack);
    }
}

void MapScene::cutSectorBetween(const QPointF& p1, const QPointF& p2)
{
    bool found(false);
    QLineF splittingLine(p1, p2);
    QPainterPath cutPath(p1);
    cutPath.lineTo(p2.x(), p2.y());

    for (int i(0); i < this->_sectors.count() && !found; i++)
    {
        SectorItem* item = this->_sectors.at(i);

        if (item->path().intersects(cutPath))
        {
            QPair<SectorItem*, SectorItem*> subSectors = item->split(splittingLine);
            qDebug() << "sector : " << i;
            if (subSectors.first != NULL || subSectors.second != NULL)
            {
                this->removeItem(item);
                this->_sectors.removeAt(i);
                emit sectorRemoved(item->competition(), i);
                delete item;

                this->addSectorItem(subSectors.first, i);
                this->addSectorItem(subSectors.second, i + 1);
                found = true;
            }
        }
    }

    if (!found)
        qDebug() << "the line doesn't cross over a sector";
}

void MapScene::manageSelectedZone(void)
{
    // Récupérer les items sélectionnés de la scéne
    QList<QGraphicsItem*> items = this->selectedItems();

    if (items.count() == 1)
    {
        CoordinateItem* itemSelected = qgraphicsitem_cast<CoordinateItem*>(items.first());

        if (itemSelected != NULL)
        {
            if (itemSelected->index().toFloat() == 0)
                qDebug("\t ---> l'index est = 0 (il faudra tjs 0 car jamais positionné");

            TrackItem* parent = qgraphicsitem_cast<TrackItem*>(itemSelected->parentItem());

            if (parent != NULL)
            {
                //emit pointSelected(itemSelected->index().toFloat(), parent->id());
                emit pointSelected(parent->getAssociateTime(itemSelected), parent->id());
            }
            else
            {
                qDebug("le track item parent n'a PAS été retrouvé");
            }
        }
        else
        {
            qDebug("Impossible de récupérer l'item sélectioné en tant que CoordinateItem");
        }
    }
    /* On a sélectionné plusieurs points, on va parcourir chaque point
     * sélectionné --> vérifier si un point de même circuit à déjà été
     * sélectionné. Si oui, on vérifie si le temps à une valeur plus grande
     * (petite) que la précédente borne supérieure (inférieure) pour ce circuit
     */
    else if (items.count() > 1)
    {
        QString msg = QString::number(items.count()) + " sont sélectionnés";
        qDebug(msg.toStdString().c_str());

        QMap< TrackItem*, QPair<float, float> > intervalsBounds;

        foreach (QGraphicsItem* item, items)
        {
            CoordinateItem* itemSelected = qgraphicsitem_cast<CoordinateItem*>(item);

            if (itemSelected != NULL)
            {
                TrackItem* parent = qgraphicsitem_cast<TrackItem*>(itemSelected->parentItem());

                if (parent != NULL)
                {
                    float time = parent->getAssociateTime(itemSelected);

                    /* Si on a déjà précédemment sélectionné un/des point(s)
                     * pour ce circuit là, on vérifie si la donnée de temps de
                     * l'élément sélectionné est supérieure à la donnée de temps
                     * maximale des précédements points pour ce circuit
                     */
                    if (intervalsBounds.contains(parent))
                    {
                        if (time <  intervalsBounds[parent].first)
                        {
                            intervalsBounds[parent].first = time;
                            qDebug() << "Parent existe, temps " << QString::number(time) + " devient borne inférieure";
                        }
                        else if (time > intervalsBounds[parent].second)
                        {
                            intervalsBounds[parent].second = time;
                            qDebug() << "Parent existe, temps " << QString::number(time) + " devient borne supérieure";
                        }
                        else
                        {
                            qDebug() << "Parent existe, temps " << QString::number(time) + " n'est aucune borne";
                        }
                    }
                    else // Aucune entrée pour ce circuit là, le temps de la coordonnées sélectionnée devient à la fois la borne inférieure et supérieure
                    {
                        intervalsBounds[parent].first = time;
                        intervalsBounds[parent].second = time;
                        qDebug() << "Parent n'existe pas, temps " << QString::number(time) + " devient borne inférieure et supérieure";
                    }
                }
            }
        }

        foreach(TrackItem* track, intervalsBounds.keys())
        {
            QPair<float, float> bounds = intervalsBounds[track];
            emit this->intervalSelected(bounds.first, bounds.second, track->id());
        }
    }
    else
    {
        qDebug("Aucun item sélectionné");
    }
}

void MapScene::highlightPoint(float timeValue, QVariant trackId)
{
    TrackItem* targetTrack(NULL);
    bool found(false);

    for (int i(0); !found && i < this->_tracks.count(); i++)
    {
        TrackItem* tr = this->_tracks[i];

        if (tr->id() == trackId)
        {
            found = true;
            targetTrack = tr;
        }
    }

    /* Vu que mnt on a une corrélation parfaite entre les vues graphiques et
     * la vue mapping, on devrait tjs trouver la trackItem correspondant
     */
    if (!found)
        return;

    CoordinateItem* nearestCoord = targetTrack->nearestCoord(timeValue);
    if (nearestCoord == NULL)
        return;

    TickItem* tick = new TickItem;
    tick->setPos(nearestCoord->pos());

    if (this->_selectedGroup == NULL)
    {
        this->_selectedGroup = new QGraphicsItemGroup;
        this->addItem(this->_selectedGroup);
    }

    this->_selectedGroup->addToGroup(tick);
}

void MapScene::highlightOnlySector(float t1, float t2, QVariant trackId)
{
    this->clearSceneSelection();
    this->highlightSector(t1, t2, trackId);
}

void MapScene::highlightSector(float t1, float t2, QVariant trackId)
{
    qDebug() << "sector délimité sur graphique: " << t1 << t2;

    TrackItem* targetTrack(NULL);
    bool found(false);

    for (int i(0); !found && i < this->_tracks.count(); i++)
    {
        TrackItem* tr = this->_tracks[i];

        if (tr->id() == trackId)
        {
            found = true;
            targetTrack = tr;
        }
    }

    if (!found)
        return;

    AnimateSectorItem* sect = targetTrack->sectorOn(t1, t2);
    if (sect == NULL)
        return;

    sect->setBrush(QBrush(Qt::red));

    if (this->_selectedGroup == NULL)
    {
        this->_selectedGroup = new QGraphicsItemGroup;
        this->addItem(this->_selectedGroup);
    }

    this->_selectedGroup->addToGroup(sect);
    sect->launchAnimation();
}

void MapScene::clearSceneSelection(void)
{
    if (this->_selectedGroup != NULL)
    {
        this->removeItem(this->_selectedGroup);
        //delete this->_selectedGroup;
        this->_selectedGroup = NULL;
    }
}

void MapScene::clearTracks(void)
{
    this->clearSceneSelection();

    // Tracks contient tous les tours affichés sur le map
    while (!this->_tracks.isEmpty())
        this->removeItem(this->_tracks.takeFirst());

    foreach (QGraphicsItemGroup* gr, this->_symbols.values())
        this->removeItem(gr);

    this->_symbols.clear();
}

void MapScene::clearSectors(void)
{
    while (!this->_sectors.isEmpty())
        this->removeItem(this->_sectors.takeFirst());
}

void MapScene::clear(void)
{
    this->clearSceneSelection();
    this->_sectors.clear();
    this->_tracks.clear();
}

void MapScene::enableTrackAcceptHoverEvents(bool enable)
{
    for (int i(0); i < this->_tracks.count(); i++)
        this->_tracks[i]->setAcceptHoverEvents(enable);

    this->_trackAcceptHoverEvents = enable;
}

void MapScene::addSectorItem(SectorItem* sect, int index)
{
    sect->setZValue(-1);
    this->_sectors.insert(index, sect);
    this->addItem(sect);

    this->reorderSectorColors();

    IndexedPosition realFirstCoord(sect->first());
    realFirstCoord.setX(sect->first().x() / this->_amplificationRatio);
    realFirstCoord.setY(sect->first().y() / -this->_amplificationRatio);

    IndexedPosition realLastCoord(sect->last());
    realLastCoord.setX(sect->last().x() / this->_amplificationRatio);
    realLastCoord.setY(sect->last().y() / -this->_amplificationRatio);
    emit sectorAdded(sect->competition(), index, realFirstCoord, realLastCoord);
}

void MapScene::reorderSectorColors(void) const
{
    ColorPicker picker(6);

    for (int i(0); i < this->_sectors.count(); i++)
    {
        SectorItem* sector = this->_sectors.at(i);
        QColor color = picker.color(i % 6, 150);
        sector->setBrush(QBrush(color));

        QPen pen(picker.dark(i % 6));
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidthF(1.5);
        pen.setCosmetic(true);
        sector->setPen(pen);
    }

}
