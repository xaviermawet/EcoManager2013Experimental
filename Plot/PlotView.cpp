#include "PlotView.hpp"

PlotView::PlotView(QWidget* parent) :
    QGraphicsView(parent), rubberLine(), posLabel(NULL)
{
    this->init();
}

PlotView::PlotView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(parent), rubberLine(), posLabel(NULL)
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
    if (!delimiting)
    {
        QRectF newScene = scene()->selectionArea().boundingRect();
        sceneStack.push(sceneRect());

        setSceneRect(newScene);
        this->updateSceneRect(newScene);
    }
}

void PlotView::zoomOut(void)
{
    if (!this->sceneStack.isEmpty())
    {
        setSceneRect(sceneStack.top());
        this->updateSceneRect(sceneStack.pop());
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
    switch (this->dragMode())
    {
        // Mode de sélection d'une zone
        case QGraphicsView::RubberBandDrag:
            if (this->delimiting)
            {
                QPainterPath path;
                path.addRect(rect);
                path.addPath(scene()->selectionArea());

                QColor bckcolor = this->foregroundBrush().color();
                bckcolor.setAlpha(175);
                painter->setBrush(bckcolor);
                painter->drawPath(path);
            }
            break;
        // Mode de déplacement de la ligne verticale
        case QGraphicsView::NoDrag:
            break;
        default:
            break;
    }
}

void PlotView::mouseMoveEvent(QMouseEvent *event)
{
    switch (this->dragMode())
    {
        // Mode de sélection d'une zone
        case QGraphicsView::RubberBandDrag:
            if (this->clicked)
            {
                delimiting = true;
                this->viewport()->update();
            }
            break;
        // Mode de déplacement de la ligne verticale
        case QGraphicsView::NoDrag:
        qDebug() << "On bouge la ligne verticale";
            break;
        default:
            break;
    }

    // Affichage des coordonnées en haut à droit du graphique
    QPointF sceneMousePoint = mapToScene(event->pos());
    this->posLabel->setText(QString("%1, %2").arg(sceneMousePoint.x(), 6, 'f', 2).arg(sceneMousePoint.y(), 6, 'f', 2));
    this->posLabel->adjustSize();
    this->posLabel->move(this->width() - this->posLabel->width() - 10, 10);
    QGraphicsView::mouseMoveEvent(event);
}

void PlotView::mousePressEvent(QMouseEvent *event)
{
    switch (this->dragMode())
    {
        // Mode de sélection d'une zone
        case QGraphicsView::RubberBandDrag:
            this->clicked = true;
            this->scene()->clearSelection();
            emit this->beginSelection();
            break;
        // Mode de déplacement de la ligne verticale
        case QGraphicsView::NoDrag:
            break;
        default:
            break;
    }

    QGraphicsView::mousePressEvent(event);
}

void PlotView::mouseReleaseEvent(QMouseEvent *event)
{
    switch (this->dragMode())
    {
        // Mode de sélection d'une zone
        case QGraphicsView::RubberBandDrag:
            this->clicked = false;
            if (this->delimiting)
            {
                this->delimiting = false;
                this->selectionChanged();
            }
            emit this->finishSelection();
            break;
        // Mode de déplacement de la ligne verticale
        case QGraphicsView::NoDrag:
            break;
        default:
            break;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void PlotView::resizeEvent(QResizeEvent*)
{
    this->updateSceneRect(sceneRect());
    posLabel->move(width() - posLabel->width() - 10, 10);
}

void PlotView::init(void)
{
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
