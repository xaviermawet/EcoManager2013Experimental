#include "PlotFrame.hpp"
#include "ui_PlotFrame.h"

PlotFrame::PlotFrame(QWidget *parent) :
    QFrame(parent), ui(new Ui::PlotFrame), plotScene(NULL), plotView(NULL)
{
    // GUI Configuration
    ui->setupUi(this);

    // Create scene
    this->plotScene = new PlotScene;

    // Create view
    this->plotView = new PlotView(this->plotScene);
    this->plotView->setAutoFillBackground(true);
    this->plotView->setRenderHint(QPainter::Antialiasing);
    this->plotView->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);

    // Layouts configuration
    this->topScaleLayout    = new QVBoxLayout;
    this->bottomScaleLayout = new QVBoxLayout;
    this->leftScaleLayout   = new QHBoxLayout;
    this->rightScaleLayout  = new QHBoxLayout;

    this->ui->plotLayout->addLayout(this->topScaleLayout, 0, 1, 1, 3);
    this->ui->plotLayout->addLayout(this->bottomScaleLayout, 5, 1, 1, 3);
    this->ui->plotLayout->addLayout(this->leftScaleLayout, 1, 0, 3, 1);
    this->ui->plotLayout->addLayout(this->rightScaleLayout, 1, 5, 3, 1);
    this->ui->plotLayout->addWidget(this->plotView, 1, 1, 3, 3);

    // Manage tool button state
    this->ui->showCurvesToolButton->setChecked(
                this->plotScene->curvesAreVisible());
    this->ui->showPointsToolButton->setChecked(
                this->plotScene->pointsAreVisible());
    this->ui->showLineToolButton->setChecked(
                this->plotScene->curveLabelsAreVisible());

    // Connect GUI signals to slots | Use autoconnect and call scene slots ?
    connect(this->ui->showCurvesToolButton, SIGNAL(toggled(bool)),
            this->plotScene, SLOT(setCurvesVisible(bool)));
    connect(this->ui->showPointsToolButton, SIGNAL(toggled(bool)),
            this->plotScene, SLOT(setPointsVisible(bool)));
    connect(this->ui->zoomOutToolButton, SIGNAL(clicked()),
            this->plotView, SLOT(zoomOut()));

    // Connect signals and slots of view and scene
    connect(this->plotView, SIGNAL(rectChange(QRectF)),
            this, SLOT(adaptScales(QRectF)));
    connect(this->plotView, SIGNAL(beginSelection()),
            this->plotScene, SLOT(lockSelectionAbility()));
    connect(this->plotView, SIGNAL(finishSelection()),
            this->plotScene, SLOT(unlockSelectionAbility()));
    connect(this->plotView, SIGNAL(mousePosChanged(QPointF,QPointF)),
            this->plotScene, SLOT(displayLabels(QPointF,QPointF)));
    connect(this->plotScene, SIGNAL(selectionChanged()),
            this, SIGNAL(selectionChanged()));

    setMouseTracking(true);
}

PlotFrame::~PlotFrame(void)
{
    delete ui;
}

void PlotFrame::on_eraseToolButton_clicked(void)
{
    emit this->clear();
}

void PlotFrame::on_showLineToolButton_toggled(bool checked)
{
    // Change the plotView dragMode
    this->plotView->setVerticalLineVisible(checked);

    // Display curves labels
    this->plotScene->setCurveLabelsVisible(checked);
}

void PlotFrame::adaptScales(const QRectF& newRect)
{
    emit this->minVChanged(newRect.top());
    emit this->maxVChanged(newRect.bottom());
    emit this->minHChanged(newRect.left());
    emit this->maxHChanged(newRect.right());
}
