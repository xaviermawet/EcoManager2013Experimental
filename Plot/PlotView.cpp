#include "PlotView.hpp"

PlotView::PlotView(QWidget* parent) :
    QGraphicsView(parent), posLabel(NULL)
{
    this->init();
}

PlotView::PlotView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(parent)
{
    this->init();
    this->setScene(scene);
}

PlotView::~PlotView()
{
}

void PlotView::updateSceneRect(const QRectF &rect)
{
    this->fitInView(rect, Qt::KeepAspectRatio);
    emit rectChange(globalRect());
}

void PlotView::selectionChanged(void)
{
    if (! delimiting )
    {
        QRectF newScene = scene()->selectionArea().boundingRect();
        sceneStack.push(sceneRect());

        setSceneRect(newScene);
        fitInView(newScene, Qt::KeepAspectRatio);
        emit rectChange(globalRect());
    }
}

void PlotView::zoomOut(void)
{
    if (! sceneStack.isEmpty())
    {
        setSceneRect(sceneStack.top());
        fitInView(sceneStack.pop(), Qt::KeepAspectRatio);
        emit rectChange(globalRect());
    }
    else
    {
        setSceneRect(scene()->sceneRect());
    }
}

void PlotView::toggleSelectionMode(void)
{
    this->setDragMode((this->dragMode() == QGraphicsView::ScrollHandDrag) ?
                 QGraphicsView::RubberBandDrag : QGraphicsView::ScrollHandDrag);
}

void PlotView::drawForeground(QPainter *painter, const QRectF &rect)
{
    if (delimiting)
    {
        QPainterPath path;
        path.addRect(rect);
        path.addPath(scene()->selectionArea());

        QColor bckcolor = this->foregroundBrush().color();
        bckcolor.setAlpha(175);
        painter->setBrush(bckcolor);
        painter->drawPath(path);
    }
}

void PlotView::mouseMoveEvent(QMouseEvent *event)
{
//    emit positionChange(event->pos());

    if (dragMode() == QGraphicsView::RubberBandDrag)
    {
        if (clicked)
        {
            delimiting = true;
            /* Par défaut, QGraphicsView utilise un QWidget standard pour sa
             * zone d'affichage. Vous pouvez accéder à ce widget en appelant
             * viewport() */
            viewport()->update();
        }
    }
    else
    {
//        horizontalScrollBar()->setValue(event->posF().x());
//        verticalScrollBar()->setValue(event->posF().y());

        // On ne viendra jamais ici ????
        //QMessageBox::information(this, "dragMode", "pas en rubberBandDrag -- one ne verra jamais ce message 2");
    }

    // Affichage des coordonnées en haut à droit du graphique
    QPointF sceneMousePoint = mapToScene(event->pos());
    posLabel->setText(QString("%1, %2").arg(sceneMousePoint.x(), 6, 'f', 2).arg(sceneMousePoint.y(), 6, 'f', 2));
    posLabel->adjustSize();
    posLabel->move(width() - posLabel->width() - 10, 10);
    QGraphicsView::mouseMoveEvent(event);
}

void PlotView::mousePressEvent(QMouseEvent *event)
{
    if (dragMode() == QGraphicsView::RubberBandDrag)
    {
        this->clicked = true;
        this->scene()->clearSelection();
        emit this->beginSelection();
    }
    else
    {
        // On ne viendra jamais ici ????
        QMessageBox::information(this, "dragMode", "pas en rubberBandDrag -- one ne verra jamais ce message 1");
        moving = true;
    }

    QGraphicsView::mousePressEvent(event);
}

void PlotView::mouseReleaseEvent(QMouseEvent *event)
{
    if (dragMode() == QGraphicsView::RubberBandDrag)
    {
        clicked = false;

        if (delimiting)
        {
            delimiting = false;
            selectionChanged();
        }

        emit finishSelection();
    }
    else
    {
        // On ne viendra jamais ici ????
        QMessageBox::information(this, "dragMode", "pas en rubberBandDrag -- one ne verra jamais ce message 3");
        moving = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void PlotView::resizeEvent(QResizeEvent*)
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    emit rectChange(globalRect());
    posLabel->move(width() - posLabel->width() - 10, 10);
}

void PlotView::init(void)
{
    clicked = false;
    delimiting = false;

    QPalette palette;
    palette.setBrush(QPalette::Highlight, Qt::white);
    this->setPalette(palette);

    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setOptimizationFlag(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignBottom); // relative Alignment of scene through view
    scale(1, -1);

    setInteractive(true); // !!!!!!!!!!!!!!!!! (true est la valeur par défaut --> cette instruction ne fait rien ! )
    setMouseTracking(true); // !!!!!!!!!!!!!!!!! le widget reçoit les événements de mouvements de souris même si aucun bouton n'est pressé
    setDragMode(QGraphicsView::RubberBandDrag);

    posLabel = new QLabel("(0, 0)", this);
    posLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    posLabel->setStyleSheet("background-color : qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(100, 100, 100, 180), stop: 1 rgb(0, 0, 0, 180)); margin : 4px; color: white; border-radius: 2px");
}

QRectF PlotView::globalRect(void) const
{
    return mapToScene(viewport()->rect()).boundingRect();
}
