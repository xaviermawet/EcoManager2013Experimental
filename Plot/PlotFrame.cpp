#include "PlotFrame.hpp"
#include "ui_PlotFrame.h"

PlotFrame::PlotFrame(QWidget *parent) :
    QFrame(parent), ui(new Ui::PlotFrame), plotScene(NULL), plotView(NULL)
{
    // GUI Configuration
    ui->setupUi(this);

    // Create scene
    this->plotScene = new PlotScene(this);

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
    connect(this->plotView, SIGNAL(mousePressed(QPointF)),
            this->plotScene, SLOT(slotDeTest(QPointF)));
    connect(this->plotView, SIGNAL(zoomedAround(int)),
            this, SLOT(zoomAround(int)));

    setMouseTracking(true);
}

PlotFrame::~PlotFrame(void)
{
    delete this->ui;
    delete this->plotScene;
    delete this->plotView;
}

PlotView* PlotFrame::view(void) const
{
    return this->plotView;
}

PlotScene* PlotFrame::scene(void) const
{
    return this->plotScene;
}

void PlotFrame::addVerticalAxis(VerticalScale* scale)
{
    if (scale->placement() == Scale::Left)
        this->leftScaleLayout->addWidget(scale);
    else
        this->rightScaleLayout->addWidget(scale);

    connect(this, SIGNAL(minVChanged(float)), scale, SLOT(setMin(float)));
    connect(this, SIGNAL(maxVChanged(float)), scale, SLOT(setMax(float)));
}

void PlotFrame::addHorizontalAxis(HorizontalScale* scale)
{
    if (scale->placement() == Scale::Bottom)
        this->bottomScaleLayout->addWidget(scale);
    else
        this->topScaleLayout->addWidget(scale);

    connect(this, SIGNAL(minHChanged(float)), scale, SLOT(setMin(float)));
    connect(this, SIGNAL(maxHChanged(float)), scale, SLOT(setMax(float)));
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

void PlotFrame::on_printToolButton_clicked(void)
{
    if (this->plotScene->items().count() <= 0)
    {
        QMessageBox::information(this, tr("Impression impossible"),
                                 tr("Aucune courbe à imprimer"));
        return;
    }

    /*
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    QPainter painter(&printer); // NOTE : au lieu de placer un QPrinter, passer un QImage pour une sauvegarde en photo

    // print, fitting the viewport contents into a full page
    this->plotView->render(&painter);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted)
    {
        QMessageBox::information(this, "Impression du graphique",
                                 "Impression du graphique");
    }
    */

    QImage img;
    QPainter painter(&img);

    // print, fitting the viewport contents into a full page
    this->plotView->render(&painter);

    img.save(QFileDialog::getSaveFileName());

}

void PlotFrame::on_eraseSelectionToolButton_clicked(void)
{
    this->plotScene->clearPlotSelection();
}

void PlotFrame::on_zoomSlider_valueChanged(int value)
{
    qDebug() << "Graphique : valeur du slider change à " << value;

    qreal scaleFactor =  qPow(2, (value - 50) / 10.0);

    qDebug() << "scaleFactor : " << scaleFactor;
}

void PlotFrame::adaptScales(const QRectF& newRect)
{
    emit this->minVChanged(newRect.top());
    emit this->maxVChanged(newRect.bottom());
    emit this->minHChanged(newRect.left());
    emit this->maxHChanged(newRect.right());
}

void PlotFrame::zoomAround(int level)
{
    this->ui->zoomSlider->setValue(this->ui->zoomSlider->value() + level * 2);
}
