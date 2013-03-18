#include "PlotPrintDialog.hpp"
#include "ui_PlotPrintDialog.h"

PlotPrintDialog::PlotPrintDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::PlotPrintDialog)
{
    // GUI Configuration
    ui->setupUi(this);

    // Default type is printing mode, so we hide the picture groupbox
    this->ui->pictureGroupBox->setVisible(false);
}

PlotPrintDialog::~PlotPrintDialog(void)
{
    delete this->ui;
}

PlotPrintDialog::printType PlotPrintDialog::getPrintType(void) const
{
    switch (this->ui->printTypeComboBox->currentIndex())
    {
    case INDEX_PAPER:
        return PlotPrintDialog::printing;
    case INDEX_PICTURE:
        return PlotPrintDialog::picture;
    case INDEX_PDF:
        return PlotPrintDialog::pdf;
    default:
        return PlotPrintDialog::printing;
    }
}

bool PlotPrintDialog::printWithScales(void) const
{
    return this->ui->withScaleCheckBox->isChecked();
}

bool PlotPrintDialog::printCurrentView(void) const
{
    return this->ui->currentViewCheckBox->isChecked();
}

bool PlotPrintDialog::printEntireView(void) const
{
    return !this->printCurrentView();
}

int PlotPrintDialog::pictureQuality(void) const
{
    return this->ui->pictureQualitySlider->value();
}

QPrinter::PrinterMode PlotPrintDialog::printerMode(void) const
{
    return (QPrinter::PrinterMode)this->ui->printerModeComboBox->currentIndex();
}

QPrinter::PaperSize PlotPrintDialog::paperSize(void) const
{
    return (QPrinter::PaperSize) this->ui->paperSizeComboBox->currentIndex();
}

QPrinter::Orientation PlotPrintDialog::paperOrientation(void) const
{
    return (QPrinter::Orientation) this->ui->paperOrientationComboBox->currentIndex();
}

void PlotPrintDialog::on_withScaleCheckBox_stateChanged(int state)
{
    Q_UNUSED(state);

    bool withScale = this->ui->withScaleCheckBox->isChecked();

    this->ui->warningLabel->setVisible(withScale);
    this->ui->currentViewCheckBox->setChecked(withScale);
    this->ui->globalViewCheckBox->setEnabled(!withScale);
}

void PlotPrintDialog::on_printTypeComboBox_currentIndexChanged(int index)
{
    switch (index)
    {
        case INDEX_PAPER:
            this->ui->printerGroupBox->setVisible(true);
            this->ui->pictureGroupBox->setVisible(false);
            break;
        case INDEX_PICTURE:
            this->ui->printerGroupBox->setVisible(false);
            this->ui->pictureGroupBox->setVisible(true);
            break;
        case INDEX_PDF:
            this->ui->printerGroupBox->setVisible(false);
            this->ui->pictureGroupBox->setVisible(false);
            break;
        default:
            break;
    }
}
