#include "PlotView.hpp"

PlotView::PlotView(QWidget* parent) :
    QGraphicsView(parent), posLabel(NULL), _numScheduledScalings(0)
{
    this->init();
}

PlotView::PlotView(QGraphicsScene* scene, QWidget* parent) :
    QGraphicsView(parent), posLabel(NULL), _numScheduledScalings(0)
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

void PlotView::scalingTime(qreal x)
{
    Q_UNUSED(x);

    qreal factor = 1.0 + qreal(this->_numScheduledScalings) / 300.0;
    scale(factor, factor);

    QRectF newScene = this->viewport()->childrenRect();
    setSceneRect(newScene);
    emit rectChange(globalRect());
}

void PlotView::animFinished(void)
{
    if (_numScheduledScalings > 0)
        _numScheduledScalings--;
    else
        _numScheduledScalings++;

    // Of course, we need to take care of dynamically created QTimeLines
    sender()->~QObject();
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
        case QGraphicsView::ScrollHandDrag:
            if (this->viewMoving)
            {
                QRectF newScene = this->viewport()->childrenRect();
                setSceneRect(newScene);
                emit rectChange(globalRect());
            }
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
        case QGraphicsView::ScrollHandDrag:
            this->viewMoving = true;
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
        case QGraphicsView::ScrollHandDrag:
            this->viewMoving = false;
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
    // Smooth zoom copied from http://qt-project.org/wiki/SmoothZoomInQGraphicsView
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;  // see QWheelEvent documentation
        this->_numScheduledScalings += numSteps;

        // if user moved the wheel in another direction,
        // we reset previously scheduled scalings
        if (this->_numScheduledScalings * numSteps < 0)
                this->_numScheduledScalings = numSteps;

        // Will invoke scalingTime function every 20ms during his 350ms lifespan
        QTimeLine* anim = new QTimeLine(350, this);
        anim->setUpdateInterval(20);

        connect(anim, SIGNAL(valueChanged(qreal)), SLOT(scalingTime(qreal)));
        connect(anim, SIGNAL(finished()), SLOT(animFinished()));
        anim->start();
    }

/*
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
*/
}

void PlotView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        qDebug() << "Changer le drag mode en ScrollHandDrag";

        // Save the current drag mode
        this->oldDragMode = this->dragMode();
        this->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void PlotView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        qDebug() << "Restaurer l'ancien drag mode"; //this->setDragMode(QGraphicsView::ScrollHandDrag);

        // Restore the previous drag mode
        this->setDragMode(this->oldDragMode);
    }
}

void PlotView::dragMoveEvent(QDragMoveEvent* event)
{
    qDebug() << "dragMoveEvent ------------------";
}

void PlotView::zoom(qreal factor, const QPointF& centerPoint)
{
    scale(factor, factor);
    centerOn(centerPoint);
}

void PlotView::init(void)
{
    this->delimiting = false;
    this->clicked = false;
    this->viewMoving = false;

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

    setInteractive(true);
    setMouseTracking(true);
    setDragMode(QGraphicsView::RubberBandDrag);
    this->oldDragMode = this->dragMode();

    posLabel = new QLabel("(0, 0)", this);
    posLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    posLabel->setStyleSheet("background-color : qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(100, 100, 100, 180), stop: 1 rgb(0, 0, 0, 180)); margin : 4px; color: white; border-radius: 2px");
}

QRectF PlotView::globalRect(void) const
{
    return mapToScene(viewport()->rect()).boundingRect();
}
