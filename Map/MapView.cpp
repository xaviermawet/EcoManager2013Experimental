#include "MapView.hpp"

MapView::MapView(QWidget *parent) :
    QGraphicsView(parent), delimiting(false), rubberLine()
{
    this->setBackgroundBrush(QColor(0, 0, 0, 180));
}

MapView::~MapView(void)
{
}

void MapView::drawForeground(QPainter* painter, const QRectF& rect)
{
    if (this->delimiting)
        painter->drawLine(this->mapToScene(this->rubberLine.p1()),
                          this->mapToScene(this->rubberLine.p2()));

    QGraphicsView::drawForeground(painter, rect);
}

// Evenement de la molette de la souris
void MapView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->delta() > 0) // On zoom avec la molette
            emit zoomedAround(1, event->pos());
        else // On dezoom
            emit zoomedAround(-1, event->pos());
        event->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    /* Si on est dans le mode NoDrag (mode pour segmenter les secteurs)
     * Affectation du point de départ à la position actuelle de la souris
     * Position du flag renseignant qu'on est en train de délimiter un secteur
     */
    switch (this->dragMode())
    {
        case QGraphicsView::NoDrag: // Delimiting sector
            if (!this->delimiting)
            {
                this->rubberLine.setPoints(event->pos(), event->pos());
                this->delimiting = true;
                this->setCursor(Qt::CrossCursor);
                event->accept();
            }
            break;
        case QGraphicsView::RubberBandDrag: // zone or point selection
            this->scene()->clearSelection();
            this->setCursor(Qt::CrossCursor);
        default:
            QGraphicsView::mousePressEvent(event);
            break;
    }
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (this->delimiting) // Si on délimite un secteur
    {
        this->viewport()->update(); // Remplace updateLineZone();
        this->rubberLine.setP2(event->pos()); // Changement du point de destination de la ligne de segmentation
        event->accept(); // Event will not be propagated to the parent widget
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MapView::mouseReleaseEvent(QMouseEvent* event)
{
    switch (this->dragMode())
    {
        case QGraphicsView::NoDrag: // Delimiting sectors
            if (this->delimiting)
            {
                this->delimiting = false;
                this->unsetCursor();
                this->viewport()->update(); // Remplace updateLineZone();

                /* Si le point au départ lors du mousePressEvent est différent du
                 * point d'arrivée lors du mouseReleaseEvent --> on a bien tiré
                 * une ligne */
                if (qAbs(rubberLine.dx()) + qAbs(rubberLine.dy()) > 2)
                {
                    emit this->areaDelimited(this->mapToScene(rubberLine.p1()),
                                             this->mapToScene(rubberLine.p2()));
                    event->accept();
                }
            }
            break;
        case QGraphicsView::RubberBandDrag: // zone or point selection
            this->unsetCursor();
            emit this->pointOrZoneSelected();
            break;
        default:
            break;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
