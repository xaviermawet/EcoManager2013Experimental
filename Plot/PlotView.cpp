#include "PlotView.hpp"

PlotView::PlotView(QWidget* parent) :
    QGraphicsView(parent), posLabel(NULL)
{
    this->init();
}

PlotView::PlotView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(parent), posLabel(NULL)
{
    this->init();
    this->setScene(scene);
}

PlotView::~PlotView()
{
}

void PlotView::updateSceneRect(const QRectF &rect)
{
    //this->fitInView(rect, Qt::KeepAspectRatio);
    this->fitInView(rect, Qt::IgnoreAspectRatio);
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

void PlotView::setVerticalLineVisible(bool visible)
{
    if (visible)
    {
        // Mode used to display vertical line
        this->setDragMode(QGraphicsView::NoDrag);
    }
    else
    {
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->viewport()->update();
    }
}

//void PlotView::toggleSelectionMode(void)
//{
//    if (this->dragMode() == QGraphicsView::NoDrag)
//    {
//        // Mode de sélection d'une zone (zoom)
//        this->setDragMode(QGraphicsView::RubberBandDrag);
//        this->viewport()->update();
//        emit this->verticalLineVisibilityChanged(false);
//    }
//    else
//    {
//        // Mode déplacement de la ligne verticale
//        this->setDragMode(QGraphicsView::NoDrag);
//        emit this->verticalLineVisibilityChanged(true);
//    }
//}

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
            painter->setPen(QPen(Qt::black, 1));

            // Draw Horizontal line
            //painter->drawLine(this->sceneRect().left(),  this->mousePos.y(),
            //                  this->sceneRect().right(), this->mousePos.y());

            // Draw vertical line
            painter->drawLine(this->mousePos.x(), this->scene()->sceneRect().top(),
                              this->mousePos.x(), this->scene()->sceneRect().bottom());
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
            this->mousePos = this->mapToScene(event->pos());
            emit this->mousePosChanged(event->posF(), this->mousePos);
            this->viewport()->update();
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
    if (event->button() != Qt::LeftButton) return;

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

//void PlotView::keyPressEvent(QKeyEvent *event)
//{
//    if(event->type() == QKeyEvent::KeyPress)
//    {
//        // CTRL + L is used to change the dragMode
//        if (event->modifiers() == Qt::ControlModifier &&
//            event->key() == Qt::Key_L)
//        {
//            bool showVerticalLine = this->dragMode() == QGraphicsView::RubberBandDrag;
//            this->setVerticalLineVisible(showVerticalLine);
//            emit this->verticalLineVisibilityChanged(showVerticalLine);
//        }
//    }

//    QGraphicsView::keyPressEvent(event);
//}

void PlotView::init(void)
{
    this->delimiting = false;
    this->clicked = false;

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
