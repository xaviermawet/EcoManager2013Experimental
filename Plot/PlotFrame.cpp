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

    // Display printing options dialog
    PlotPrintDialog ppd;
    if (ppd.exec() == QDialog::Rejected)
        return;

    // Printing options
    switch (ppd.getPrintType())
    {
        case PlotPrintDialog::printing:
        {
            // Printer configuration
            QPrinter printer(ppd.printerMode());
            printer.setPaperSize(ppd.paperSize());
            printer.setOrientation(ppd.paperOrientation());

            // Printer dialog to choose a printer
            QPrintDialog printDialog(&printer);
            if (printDialog.exec() == QDialog::Rejected)
            {
                QMessageBox::information(this, tr("Impression interrompue"),
                             tr("L'impression à correctement été interrompue"));
                break;
            }

            // Printing ...
            QPainter painter;
            painter.setRenderHint(QPainter::Antialiasing);
            painter.begin(&printer);

            if(ppd.printWithScales())
            {
                // Enlever les boutons d'options
                QRect rectLayoutToolButtons = this->ui->toolButtonsHLayout->geometry();
                this->ui->toolButtonsHLayout->setGeometry(QRect(0, 0, 0, 0));

                double xscale = printer.pageRect().width() / double(this->width());
                double yscale = printer.pageRect().height() / double(this->height());
                double scale = qMin(xscale, yscale);
                painter.translate(printer.paperRect().x() + printer.pageRect().width() / 2,
                                  printer.paperRect().y() + printer.pageRect().height() / 2);
                painter.scale(scale, scale);
                painter.translate(-width() / 2, -height() / 2);

                /* Modifie flag pour ne pas imprimer la couleur de
                 * fond des widgets parnets et enfants */
                this->render(&painter, QPoint(), QRegion(),
                             QWidget::DrawChildren);

                // Restore layout geometry
                this->ui->toolButtonsHLayout->setGeometry(rectLayoutToolButtons);
            }
            else
            {
                if (ppd.printCurrentView())
                    this->plotView->render(&painter, QRectF(), QRect(),
                                           Qt::IgnoreAspectRatio);
                else
                    this->plotScene->render(&painter, QRectF(), QRectF(),
                                            Qt::IgnoreAspectRatio);
            }

            painter.end();
            break;
        }
        case PlotPrintDialog::picture:
        {
            QString fileName = QFileDialog::getSaveFileName(
                        this, tr("Sauvegarder un image du graphique"),
                        QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
            QFileInfo fileInfo(fileName);

            if (ppd.printWithScales())
            {
                // Enlever les boutons d'options
                QRect rectLayoutToolButtons = this->ui->toolButtonsHLayout->geometry();
                this->ui->toolButtonsHLayout->setGeometry(QRect(0, 0, 0, 0));

                QPixmap pixmap(this->size());
                this->render(&pixmap);

                // Restore layout geometry
                this->ui->toolButtonsHLayout->setGeometry(rectLayoutToolButtons);

                pixmap.save(fileInfo.absoluteFilePath(),
                            fileInfo.suffix().toStdString().c_str(),
                            ppd.pictureQuality());
            }
            else
            {
                if (ppd.printCurrentView()) // Marche
                {
                    QImage image(QSize(1920, 1080), QImage::Format_ARGB32);
                    QPainter painter;
                    painter.begin(&image);
                    painter.setRenderHint(QPainter::Antialiasing);

                    this->plotView->render(&painter, QRectF(), QRect(),
                                           Qt::IgnoreAspectRatio);
                    image.save(fileInfo.absoluteFilePath(),
                               fileInfo.suffix().toStdString().c_str(),
                               ppd.pictureQuality());
                    painter.end();
                }
                else
                {
                    QImage image(QSize(1920, 1080), QImage::Format_ARGB32);
                    QPainter painter;
                    painter.begin(&image);
                    painter.setRenderHint(QPainter::Antialiasing);



                    this->plotScene->render(&painter, QRectF(), QRectF(),
                                            Qt::IgnoreAspectRatio);

                    image = image.mirrored();
                    image.save(fileInfo.absoluteFilePath(),
                               fileInfo.suffix().toStdString().c_str(),
                               ppd.pictureQuality());
                    painter.end();
                }
            }

            break;
        }
        default:
            QMessageBox::warning(this, tr("Impression impossible"),
                                 tr("Mode d'impression non supporté"));
            break;
    }
}

void PlotFrame::on_eraseSelectionToolButton_clicked(void)
{
    this->plotScene->clearPlotSelection();
}

void PlotFrame::on_restoreDefaultPlotSizeToolButton_clicked(void)
{
    this->plotView->restoreSceneRect();
}

void PlotFrame::adaptScales(const QRectF& newRect)
{
    emit this->minVChanged(newRect.top());
    emit this->maxVChanged(newRect.bottom());
    emit this->minHChanged(newRect.left());
    emit this->maxHChanged(newRect.right());
}
