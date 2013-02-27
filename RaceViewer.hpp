#ifndef __RACEVIEWER_HPP__
#define __RACEVIEWER_HPP__

#include "ExtensibleEllipseItem.hpp"
#include "DBModule/GeoCoordinate.hpp"
#include "Map/MapView.hpp"
#include <QtGui>

class ResizableView : public QGraphicsView
{
    public:

        explicit ResizableView(QWidget *parent);

    protected:

        virtual void wheelEvent(QWheelEvent *event);
};

class RaceViewer : public QDialog
{
    Q_OBJECT

    public:

        explicit RaceViewer(const QList<GeoCoordinate>& racePoints,
                            QWidget *parent = 0);

        // Getter
        QList< QPair<QTime, QTime> > laps(void) const;

    public slots:

        virtual void accept(void);

    private:

        void cutLaps(void);

        QList<GeoCoordinate> mPoints;
        QList< QPair<QTime, QTime> > mLaps;
        QGraphicsScene* scene;
        ResizableView* view;
        QGraphicsEllipseItem* marker;
};

#endif /* __RACEVIEWER_HPP__ */
