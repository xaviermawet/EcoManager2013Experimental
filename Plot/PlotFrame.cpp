#include "PlotFrame.hpp"

PlotFrame::PlotFrame(QFrame *parent) :
    QFrame(parent)
{
    _scene = new QGraphicsScene;
    _selectionLocked = 0;
    _pointsVisible = true;
    _curveVisible = true;
    _selectedGroup = NULL;
    createView();

    QGridLayout* plotLayout = new QGridLayout;
    _topScaleLayout = new QVBoxLayout;
    _bottomScaleLayout = new QVBoxLayout;
    _leftScaleLayout = new QHBoxLayout;
    _rightScaleLayout = new QHBoxLayout;

    plotLayout->addLayout(_topScaleLayout, 0, 1, 1, 3);
    plotLayout->addLayout(_bottomScaleLayout, 5, 1, 1, 3);
    plotLayout->addLayout(_leftScaleLayout, 1, 0, 3, 1);
    plotLayout->addLayout(_rightScaleLayout, 1, 5, 3, 1);
    plotLayout->addWidget(_mainview, 1, 1, 3, 3);
    plotLayout->setVerticalSpacing(0);
    plotLayout->setHorizontalSpacing(0);

    createToolBar();
    QVBoxLayout* topLayout = new QVBoxLayout;
    topLayout->addWidget(_toolbar);
    topLayout->addLayout(plotLayout);
    setLayout(topLayout);

    setMouseTracking(true);

    connect(_mainview, SIGNAL(rectChange(QRectF)), this, SLOT(adaptScales(QRectF)));
    connect(_scene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
    connect(_mainview, SIGNAL(beginSelection()), this, SLOT(lockSelectionAbility()));
    connect(_mainview, SIGNAL(finishSelection()), this, SLOT(unlockSelectionAbility()));

    // ---- Connexion du signal mousePosChanger pour afficher les labels -------
    this->labelInfoPoint = new QLabel(this);
    connect(this->_mainview, SIGNAL(mousePosChanged(QPointF)), this, SLOT(displayLabels(QPointF)));
    // -------------------------------------------------------------------------
}


void PlotFrame::addCurve(PlotCurve *curve)
{
    ColorPicker picker(8);
    curve->setPen(picker.color(_curves.size() % 8)); // Modifie le "pen" de toutes les les lignes et point du tracé
    _curves.append(curve); // Ajout du tracé à la liste de tous les tracés du graphique
    _scene->addItem(curve); // Ajout du tracé à la scene --> pourra être sélectionnable etc ..
    _scene->setSceneRect(_scene->itemsBoundingRect()); // Recadrage de la scène

    curve->setPointsVisible(_pointsVisible);
    curve->setCurveVisible(_curveVisible);
}

PlotCurve* PlotFrame::addCurve(QList<QPointF> points, QVariant curveId)
{
    PlotCurve* curve = new PlotCurve(points, curveId);
    addCurve(curve);
    return curve;
}

PlotCurve *PlotFrame::addCurve(QList<IndexedPosition> points, QVariant curveId)
{
    PlotCurve* curve = new PlotCurve(points, curveId);
    addCurve(curve);
    return curve;
}

void PlotFrame::showCurves(bool visible)
{
    _curveVisible = visible;

    foreach (PlotCurve* c, _curves)
        c->setCurveVisible(visible);
}

void PlotFrame::showPoints(bool visible)
{
    _pointsVisible = visible;

    foreach (PlotCurve* c, _curves)
        c->setPointsVisible(visible);
}

void PlotFrame::clearPlotSelection(void)
{
    if (this->_selectedGroup != NULL)
    {
        this->_scene->removeItem(this->_selectedGroup);
        delete _selectedGroup;
        _selectedGroup = NULL;
    }
}

void PlotFrame::clearCurves(void)
{
    this->clearPlotSelection();
    _scene->clear();
    _curves.clear();
}

void PlotFrame::highlightPoint(float timeValue, QVariant trackId)
{
    qDebug() << "point cliqué sur mapping : " << timeValue;

    PlotCurve* targetPlotCurve = NULL;

    for (int i(0); targetPlotCurve == NULL && i < this->_curves.count(); i++)
        if (this->_curves.at(i)->id() == trackId)
            targetPlotCurve = this->_curves[i];

    if (targetPlotCurve == NULL)
    {
        qDebug("PlotCurve correspondant pas trouvé! --> pas normal");
        return;
    }

    AnimateSectorItem* sect = targetPlotCurve->sectorOn(timeValue);
    if (sect == NULL)
    {
        qDebug() << "impossible de déterminer le secteur correspondant à la sélection de la vue mapping";
        return;
    }

    //sect->setBrush(QBrush(Qt::blue));
    sect->setBrush(targetPlotCurve->getPen().brush());

    if (this->_selectedGroup == NULL)
    {
        this->_selectedGroup = new QGraphicsItemGroup;
        this->_scene->addItem(_selectedGroup);
    }

    this->_selectedGroup->addToGroup(sect);
    sect->launchAnimation();
}

void PlotFrame::highlightSector(float t1, float t2, QVariant trackId)
{
    qDebug() << "sector délimité sur mapping. Bornes : " << t1 << t2;
    PlotCurve* targetPlotCurve = NULL;

    for (int i(0); targetPlotCurve == NULL && i < this->_curves.count(); i++)
        if (this->_curves.at(i)->id() == trackId)
            targetPlotCurve = this->_curves[i];

    if (targetPlotCurve == NULL)
    {
        qDebug("PlotCurve correspondant pas trouvé! --> pas normal");
        return;
    }

    AnimateSectorItem* sect = targetPlotCurve->sectorOn(t1, t2);
    if (sect == NULL)
    {
        qDebug() << "impossible de déterminer le secteur correspondant à la sélection de la vue mapping";
        return;
    }

    //sect->setBrush(QBrush(Qt::blue));
    sect->setBrush(targetPlotCurve->getPen().brush());

    if (this->_selectedGroup == NULL)
    {
        this->_selectedGroup = new QGraphicsItemGroup;
        this->_scene->addItem(_selectedGroup);
    }

    this->_selectedGroup->addToGroup(sect);
    sect->launchAnimation();
}

void PlotFrame::displayLabels(const QPointF &mousePos)
{
    qDebug() << "mousePos x : " << mousePos.x();

    foreach (PlotCurve* curve, this->_curves)
    {
        CoordinateItem* itemAtMousePos = curve->nearestCoordinateitemsOfX(mousePos.x());

        if (itemAtMousePos)
        {
            qDebug() << "Coordinate item trouvé : " << itemAtMousePos->x()
                        << itemAtMousePos->y();

            PlotCurve* parent = qgraphicsitem_cast<PlotCurve*>(itemAtMousePos->parentItem());
            if (!parent) continue;

            // Change the label text color
            QPalette pal;
            pal.setColor(QPalette::WindowText, parent->getPen().color());
            this->labelInfoPoint->setPalette(pal);

            QPoint pos = this->_mainview->mapFromScene(mousePos);
            this->labelInfoPoint->setText(QString("%1, %2").arg(itemAtMousePos->x(), 6, 'f', 2).arg(itemAtMousePos->y(), 6, 'f', 2));
            this->labelInfoPoint->adjustSize();
            this->labelInfoPoint->move(pos.x(), pos.y());
        }
    }
}

void PlotFrame::adaptScales(const QRectF& newRect)
{
    emit minVChanged(newRect.top());
    emit maxVChanged(newRect.bottom());
    emit minHChanged(newRect.left());
    emit maxHChanged(newRect.right());
}

void PlotFrame::createView()
{
    _mainview = new PlotView(_scene);
    _mainview->setAutoFillBackground(true);
    _mainview->setRenderHint(QPainter::Antialiasing);
    _mainview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void PlotFrame::createToolBar()
{
   _toolbar = new QToolBar;
   QAction* shCurveAction = new QAction(QIcon(":/chartline.png"), tr("Afficher les courbes"), this);
   shCurveAction->setCheckable(true);
   shCurveAction->setChecked(true);

   QAction* shPointsAction = new QAction(QIcon(":/scatter.png"), tr("Afficher les points"), this);
   shPointsAction->setCheckable(true);
   shPointsAction->setChecked(true);

   QAction* shVLineAction = new QAction(QIcon(":/Line.png"), tr("Afficher ligne verticale"), this);
   shVLineAction->setCheckable(true);
   //shVLineAction->setChecked(false);

   //QAction* measureAction = new QAction(QIcon(":/icon/measure.png"), tr("Effectuer une mesure"), this);
   //measureAction->setCheckable(true);

   QAction* zoomOutAction = new QAction(QIcon(":/zoomout.png"), tr("Zoom out"), this);

   _toolbar->addAction(shCurveAction);
   _toolbar->addAction(shPointsAction);
   _toolbar->addAction(shVLineAction);
//   toolbar->addAction(measureAction);
   _toolbar->addAction(zoomOutAction);
   //toolbar->addAction(tr("Effacer"), this, SLOT(clearCurves()));
   _toolbar->addAction(tr("Effacer"), this, SIGNAL(clear()));

//   QAction* toggleMode = toolbar->addAction(tr("Drag"), mainview, SLOT(toggleSelectionMode()));
//   toggleMode->setCheckable(true);
//   toggleMode->setChecked(false);

   connect(shCurveAction, SIGNAL(toggled(bool)), this, SLOT(showCurves(bool)));
   connect(shPointsAction, SIGNAL(toggled(bool)), this, SLOT(showPoints(bool)));
   connect(shVLineAction, SIGNAL(toggled(bool)), this->_mainview, SLOT(verticalLineVisible(bool)));
   connect(this->_mainview, SIGNAL(verticalLineVisibilityChanged(bool)), shVLineAction, SLOT(setChecked(bool)));
//   connect(measureAction, SIGNAL(toggled(bool)), this, SLOT(measureMode(bool)));
   connect(zoomOutAction, SIGNAL(triggered()), _mainview, SLOT(zoomOut()));
}


void PlotFrame::handleSelection()
{
    if (_selectionLocked > 0)
        return;

    QList<QGraphicsItem*> items = _scene->selectedItems();

    /* On n'a sélectionné qu'un élément, ça n'est donc pas une selection
     * rectangulaire --> on a sélectionné un point
     */
    if (items.count() == 1)
    {
        CoordinateItem* coord = qgraphicsitem_cast<CoordinateItem*>(items.first());

        if (coord != NULL)
        {
            PlotCurve* parent = qgraphicsitem_cast<PlotCurve*>(coord->parentItem());

            if (parent != NULL)
                emit pointSelected(coord->index().toFloat(), parent->id());
        }
    }
    else if (items.count() > 1)
    {
        QMap< PlotCurve*, QPair<float, float> > intervalsBounds;

        foreach (QGraphicsItem* item, items)
        {
            CoordinateItem* coord = qgraphicsitem_cast<CoordinateItem*>(item);

            if (coord != NULL)
            {
                PlotCurve* parent = qgraphicsitem_cast<PlotCurve*>(coord->parentItem());

                if (parent != NULL)
                {
                    if (! intervalsBounds.contains(parent))
                    {
                        intervalsBounds[parent].first = coord->index().toFloat();
                        intervalsBounds[parent].second = coord->index().toFloat();
                    }
                    else
                    {
                        if (coord->index().toFloat() < intervalsBounds[parent].first)
                            intervalsBounds[parent].first = coord->index().toFloat();
                        else if (coord->index().toFloat() > intervalsBounds[parent].second)
                            intervalsBounds[parent].second = coord->index().toFloat();
                    }
                }
            }
        }

        foreach (PlotCurve* curve, intervalsBounds.keys())
        {
            QPair<float, float> bounds = intervalsBounds[curve];
            emit intervalSelected(bounds.first, bounds.second, curve->id());
        }
    }
}

void PlotFrame::lockSelectionAbility()
{
    _selectionLocked++;
}

void PlotFrame::unlockSelectionAbility()
{
    _selectionLocked--;

    if (_selectionLocked <= 0)
    {
        _selectionLocked = 0;
        handleSelection();
    }
}


void PlotFrame::addVerticalAxis(VerticalScale* scale) {
    if (scale->placement() == Scale::Left)
        _leftScaleLayout->addWidget(scale);
    else
        _rightScaleLayout->addWidget(scale);

    connect(this, SIGNAL(minVChanged(float)), scale, SLOT(setMin(float)));
    connect(this, SIGNAL(maxVChanged(float)), scale, SLOT(setMax(float)));
}

void PlotFrame::addHorizontalAxis(HorizontalScale* scale) {
    if (scale->placement() == Scale::Bottom)
        _bottomScaleLayout->addWidget(scale);
    else
        _topScaleLayout->addWidget(scale);

    connect(this, SIGNAL(minHChanged(float)), scale, SLOT(setMin(float)));
    connect(this, SIGNAL(maxHChanged(float)), scale, SLOT(setMax(float)));
}

QGraphicsScene *PlotFrame::scene() const {
    return _scene;
}
