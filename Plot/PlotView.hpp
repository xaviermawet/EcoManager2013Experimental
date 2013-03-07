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
        //void verticalLineVisibilityChanged(bool visible);

    public slots:

       void updateSceneRect(const QRectF &rect);
       void selectionChanged(void);
       void zoomOut(void);
       void setVerticalLineVisible(bool visible);

    protected:

       //void toggleSelectionMode(void);

       virtual void drawForeground(QPainter *painter, const QRectF &rect);
       virtual void mouseMoveEvent(QMouseEvent*);
       virtual void mousePressEvent(QMouseEvent*);
       virtual void mouseReleaseEvent(QMouseEvent*);
       virtual void resizeEvent(QResizeEvent*);
       //virtual void keyPressEvent(QKeyEvent *event);
       //virtual void wheelEvent(QWheelEvent*);

       void init(void);
       QRectF globalRect(void) const;

       bool clicked;
       bool delimiting;
       QPointF mousePos; // devrait etre dans une classe implémentant QGraphicsScene
       QStack<QRectF> sceneStack;
       QLabel* posLabel;
};

#endif // PLOTVIEW_HPP
