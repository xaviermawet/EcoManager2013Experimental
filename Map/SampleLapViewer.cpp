#include "SampleLapViewer.hpp"
#include "ui_SampleLapViewer.h"

SampleLapViewer::SampleLapViewer(QWidget *parent) :
    QDialog(parent), ui(new Ui::SampleLapViewer)
{
    // GUI Configuration
    ui->setupUi(this);
}

QSize SampleLapViewer::sizeHint(void)
{
    return QSize(600, 400);
}

QPair<int, int> SampleLapViewer::selectedReferencesLap(void) const
{
    return this->lapIds.at(this->ui->lapsViewStack->currentIndex());
}

SampleLapViewer::~SampleLapViewer()
{
    delete ui;
}

void SampleLapViewer::on_previousLapButton_clicked(void)
{

}

void SampleLapViewer::on_nextLapButton_clicked(void)
{

}

void SampleLapViewer::on_buttonBox_accepted(void)
{

}

void SampleLapViewer::on_buttonBox_rejected(void)
{

}

void SampleLapViewer::zoomView(int increment)
{

}

MapView* SampleLapViewer::createLapView(int refRace, int refLap)
{
    return NULL;
}

void SampleLapViewer::init(void)
{

}
