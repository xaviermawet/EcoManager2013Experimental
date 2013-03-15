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
    qDebug() << "PlotView --> updateSceneRect";

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

void PlotView::mouseMoveEvent(QMouseEvent* event)
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

void PlotView::mousePressEvent(QMouseEvent* event)
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
            // Emettre un signal avec la position de la souris dans la
            emit this->mousePressed(this->mousePos);
            break;
        default:
            break;
    }

    QGraphicsView::mousePressEvent(event);
}

void PlotView::mouseReleaseEvent(QMouseEvent* event)
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

void PlotView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    this->updateSceneRect(sceneRect());
    posLabel->move(width() - posLabel->width() - 10, 10);
}

void PlotView::wheelEvent(QWheelEvent* event)
{
    // zoom only when CTRL key pressed
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        int numSteps = event->delta() / 15 / 8;

        if (numSteps == 0)
        {
            event->ignore();
            return;
        }

        qreal sc = pow(1.025, numSteps); // I use scale factor 1.025
        this->zoom(sc, mapToScene(event->pos()));
        event->accept();
    }

    /*
    if (event->modifiers() & Qt::ControlModifier)
    {
        qDebug() << "On zoom dans les vues graphiques";
        this->centerOn(event->pos());

        emit this->zoomedAround(event->delta() > 0 ? 1 : -1);


        // Get the area of the scene visualized by this view
        QRectF rectScene = this->sceneRect();

        qDebug() << rectScene.topLeft() << " " << rectScene.topRight() << " "
                 << rectScene.bottomLeft() << " " << rectScene.bottomRight();

        // Adjust size
        QPointF tr = rectScene.topRight();
        tr.setX(tr.x() + (event->delta() > 0 ? -1 : 1));
        tr.setY(tr.y() + (event->delta() > 0 ? -1 : 1));
        rectScene.setTopRight(tr);

        QPointF tl = rectScene.topLeft();
        tl.setX(tl.x() + (event->delta() > 0 ? 1 : -1));
        tl.setY(tl.y() + (event->delta() > 0 ? -1 : 1));
        rectScene.setTopLeft(tl);

        QPointF br = rectScene.bottomRight();
        br.setX(br.x() + (event->delta() > 0 ? -1 : 1));
        br.setY(br.y() + (event->delta() > 0 ? 1 : -1));
        rectScene.setBottomRight(br);

        QPointF bl = rectScene.bottomLeft();
        bl.setX(bl.x() + (event->delta() > 0 ? 1 : -1));
        bl.setY(bl.y() + (event->delta() > 0 ? 1 : -1));
        rectScene.setBottomLeft(bl);

        qDebug() << rectScene.topLeft() << " " << rectScene.topRight() << " "
                 << rectScene.bottomLeft() << " " << rectScene.bottomRight();

        // Centre sur la souris ???

        // Update the scene rect
        this->setSceneRect(rectScene);
        this->updateSceneRect(rectScene);
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
    */
}

void PlotView::zoom(qreal factor, const QPointF &centerPoint)
{
    scale(factor, factor);
    //centerOn(centerPoint);
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
