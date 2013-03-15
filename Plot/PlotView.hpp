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
        //void verticalLineVisibilityChanged(bool visible);

    public slots:

       void updateSceneRect(const QRectF &rect);
       void selectionChanged(void);
       void zoomOut(void);
       void setVerticalLineVisible(bool visible);

    protected:

       //void toggleSelectionMode(void);

       virtual void drawForeground(QPainter *painter, const QRectF &rect);
       virtual void mouseMoveEvent(QMouseEvent* event);
       virtual void mousePressEvent(QMouseEvent* event);
       virtual void mouseReleaseEvent(QMouseEvent* event);
       virtual void resizeEvent(QResizeEvent* event);
       virtual void wheelEvent(QWheelEvent* event);
       void zoom(qreal factor, const QPointF& centerPoint);
       //virtual void keyPressEvent(QKeyEvent *event);

       void init(void);
       QRectF globalRect(void) const;

       bool clicked;
       bool delimiting;
       QPointF mousePos; // devrait etre dans une classe implémentant QGraphicsScene
       QStack<QRectF> sceneStack;
       QLabel* posLabel;
};

#endif // PLOTVIEW_HPP
