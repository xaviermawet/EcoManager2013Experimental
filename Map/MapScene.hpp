#ifndef __MAPSCENE_HPP__
#define __MAPSCENE_HPP__

#include "TrackItem.hpp"
#include "TickItem.hpp"
#include "PathBuilder.hpp"
#include "CurvePathBuilder.hpp"
#include "SectorItem.hpp"
#include "AnimateSectorItem.hpp"
#include "MapView.hpp"
#include "../Common/ColorPicker.hpp"
#include "../Common/IndexedPosition.hpp"
#include "../Common/CoordinateItem.hpp"
#include <QtGui>

class MapScene : public QGraphicsScene
{
    Q_OBJECT

    public:

        MapScene(qreal ratio = 1, QObject* parent = 0);
        virtual ~MapScene(void);

        void addSector(const QVector<IndexedPosition>& points,
                       const QString &targetCompetition = QString());
        bool hasSectors(void) const;
        void mergeSector(int fstNum, int sndNum);
        void addTrack(const QVector<QPointF>& points);
        void addTrack(const QVector<QPointF>& points,
                      const QVector<float>& metaData,
                      QVariant idTrack = QVariant());
        void attacheMapView(MapView* view);
        void fixSymbol(float timeValue, QColor color, QVariant trackId);
        void removeSymbol(QVariant trackId);

    signals:

        void sectorRemoved(QString, int);
        void sectorAdded(QString, int, IndexedPosition, IndexedPosition);
        void sectorUpdated(QString, int, IndexedPosition, IndexedPosition);

        void pointSelected(float absciss, QVariant idTrack);
        void intervalSelected(float firstAbsciss, float secondAbsciss, QVariant idTrack);

    public slots:

        void cutSectorBetween(const QPointF& p1, const QPointF& p2);
        void manageSelectedZone(void);
        void highlightPoint(float timeValue, QVariant trackId);
        void highlightSector(float t1, float t2, QVariant trackId);
        void clearSceneSelection(void);
        void clearTracks(void);
        void clearSectors(void);
        void clear(void);
        void enableTrackAcceptHoverEvents(bool enable);

    private:

        void addSectorItem(SectorItem* sect, int index);
        void reorderSectorColors(void) const;

    protected:

        qreal _amplificationRatio;
        bool _trackAcceptHoverEvents;

        QList<SectorItem*> _sectors;
        QList<TrackItem*> _tracks;
        QGraphicsItemGroup* _selectedGroup;
        QMap<int, QGraphicsItemGroup*> _symbols;
};

#endif /* __MAPSCENE_HPP__ */
