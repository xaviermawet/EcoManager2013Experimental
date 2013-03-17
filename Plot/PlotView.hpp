#ifndef __PLOTVIEW_HPP__
#define __PLOTVIEW_HPP__

#include <QtGui>

class PlotView : public QGraphicsView
{
    Q_OBJECT

    public:

        PlotView(QWidget* parent = 0);
        PlotView(QGraphicsScene* scene, QWidget* parent = 0);
        virtual ~PlotView(void);

    signals:

        void mousePressed(const QPointF& scenePos);
        void mousePosChanged(const QPointF& mousePos, const QPointF& scenePos);
        void rectChange(const QRectF&);
        void beginSelection(void);
        void finishSelection(void);
        void zoomedAround(int level);

    public slots:

       void updateSceneRect(const QRectF &rect);
       void selectionChanged(void);
       void zoomOut(void);
       void setVerticalLineVisible(bool visible);

    protected slots:

       // Zoom slots
       void scalingTime(qreal x);
       void animFinished(void);

    protected:

       virtual void drawForeground(QPainter *painter, const QRectF &rect);
       virtual void mouseMoveEvent(QMouseEvent* event);
       virtual void mousePressEvent(QMouseEvent* event);
       virtual void mouseReleaseEvent(QMouseEvent* event);
       virtual void resizeEvent(QResizeEvent* event);
       virtual void wheelEvent(QWheelEvent* event);
       virtual void keyPressEvent(QKeyEvent* event);
       virtual void keyReleaseEvent(QKeyEvent* event);
       virtual void dragMoveEvent(QDragMoveEvent* event);

       void zoom(qreal factor, const QPointF& centerPoint);

       void init(void);
       QRectF globalRect(void) const;

       bool clicked;
       bool delimiting;
       bool viewMoving;
       QPointF mousePos;
       QStack<QRectF> sceneStack;
       QLabel* posLabel;

       int _numScheduledScalings;
       DragMode oldDragMode;
};

#endif // PLOTVIEW_HPP
