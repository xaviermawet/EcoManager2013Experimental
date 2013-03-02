#include "RaceViewer.hpp"

ResizableView::ResizableView(QWidget* parent) :
    QGraphicsView(parent)
{
}

void ResizableView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        QTransform scaling;

        if (event->delta() > 0)
            scaling.scale(1.1, 1.1);
        else
            scaling.scale(0.9, 0.9);
        setTransform(scaling, true);
        event->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

RaceViewer::RaceViewer(const QList<GeoCoordinate> &racePoints, QWidget *parent)
    : QDialog(parent), mPoints(racePoints)
{
    const int ratio = 300 * 1000;
    scene = new QGraphicsScene(this);
    view  = new ResizableView(this);
    view->setScene(scene);
    view->scale(ratio, -ratio);

    QPointF prev = racePoints.at(0).projection();
    QPointF cur;
    int j = 1;

    while (j < mPoints.size())
    {
        cur = mPoints.at(j).projection();

        if (prev != cur)
        {
            QGraphicsLineItem* segment;
            segment = scene->addLine(prev.x(), prev.y(), cur.x(), cur.y());
            segment->setData(0, j - 1);
            prev = cur;
            j++;
        }
        else
        {
            mPoints.removeAt(j);
        }
    }

    qDebug() << j;

    QPointF first = racePoints.at(0).projection();
    marker = new ExtensibleEllipseItem;
    marker->setRect(first.x(), first.y(), 4 / 10000.0, 4 / 10000.0);
    marker->moveBy(-2 / 10000.0, -2 / 10000.0);

    marker->setFlag(QGraphicsItem::ItemIsMovable, true);
    marker->setBrush(QColor(255, 0, 0, 127));
    marker->setZValue(100);
    scene->addItem(marker);

    QPushButton* okButton = new QPushButton(tr("Ok"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* cancelButton = new QPushButton(tr("Annuler"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout* viewerLayout = new QHBoxLayout;
    viewerLayout->addWidget(view);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    QVBoxLayout* topLayout = new QVBoxLayout;
    topLayout->addLayout(viewerLayout);
    topLayout->addLayout(buttonLayout);
    setLayout(topLayout);
}

QList<QPair<QTime, QTime> > RaceViewer::laps(void) const
{
    return this->mLaps;
}

void RaceViewer::accept(void)
{
    // Découpe des données en différents tours
    cutLaps();

    int rc = QMessageBox::information(this, tr("Découpe des données en tours"),
                             tr("Votre sélection a permis de délimiter %1 " \
                                "tour(s)\n\nVoulez-vous continuer ?\n" \
                                "Appuyer sur Non pour refaire une sélection").arg(mLaps.size()),
                             QMessageBox::No | QMessageBox::Yes);

    if (rc == QMessageBox::Yes)
        QDialog::accept();
}

void RaceViewer::cutLaps(void)
{
    this->mLaps.clear();

    QList<QGraphicsItem*> items = marker->collidingItems(Qt::IntersectsItemShape);
    QList<QGraphicsLineItem*> segments;

    foreach(QGraphicsItem* item, items)
    {
        QGraphicsLineItem* segment = qgraphicsitem_cast<QGraphicsLineItem*>(item);

        if (segment != NULL)
        {
            QList<QGraphicsLineItem*>::iterator it = segments.begin();
            bool found(false);

            while (it != segments.end() && ! found)
            {
                if ((*it)->data(0).toInt() >= segment->data(0).toInt())
                    found = true;
                else
                    it++;
            }

            segments.insert(it, segment);
        }
    }

    int startInd(-2);
    int curInd(-2);
    QList<QTime> lapStartTimes;

    for (int i(0); i < segments.size(); i++)
    {
        int ind = segments.at(i)->data(0).toInt();

        if (curInd < ind - 1)
        {
            if (startInd >= 0)
            {
                int midInd = (startInd + curInd) / 2;

                lapStartTimes << mPoints.value(midInd).time();
//                qDebug() << "----------" << midInd << mPoints.value(midInd).time() << "------------";
            }

            startInd = ind;
            curInd = ind;
        }
        else
        {
            // curInd == ind - 1
            curInd++;
        }
    }

    if (! lapStartTimes.isEmpty())
    {
        lapStartTimes << mPoints.value(segments.last()->data(0).toInt()).time();
        lapStartTimes << mPoints.last().time();
    }

    for (int i(1); i < lapStartTimes.size(); i++)
        mLaps << QPair<QTime, QTime>(lapStartTimes.at(i - 1), lapStartTimes.at(i));
}
