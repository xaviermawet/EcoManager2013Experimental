#ifndef __MAPVIEW_HPP__
#define __MAPVIEW_HPP__

#include <QtGui>

class MapView : public QGraphicsView
{
    Q_OBJECT

    public:

        MapView(QWidget* parent = 0);
        MapView(QGraphicsScene* scene, QWidget* parent = 0);
        virtual ~MapView(void);

    signals:

        void areaDelimited(QPointF p1, QPointF p2);
        void pointOrZoneSelected(void);
        void zoomedAround(int factor, QPointF origin);

    protected:

        virtual void drawForeground(QPainter* painter, const QRectF& rect);
        virtual void mousePressEvent(QMouseEvent *);
        virtual void mouseMoveEvent(QMouseEvent *);
        virtual void mouseReleaseEvent(QMouseEvent *);
        virtual void wheelEvent(QWheelEvent *);

        bool delimiting;
        QLine rubberLine;
};

#endif /* __MAPVIEW_HPP__ */
