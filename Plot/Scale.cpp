#include "Scale.hpp"

Scale::Scale(QWidget *parent) :
    QWidget(parent), mmin(0), mmax(100), initResolution(10),
    numTicks(2), precision(2), indexHorizontal(true), minToDisplay(0),
    translation(0), scaleFactor(1)
{
    /*
     * customContextMenuRequested
     * ------------------------------
     * Signal émis lors d'un clic droit sur un widget afin d'afficher un menu
     * personnalisé à l'endroit du clic
     *
     * connect(this, SIGNAL(customContextMenuRequested(QPoint)),
     *         this, SLOT(showContextMenu(QPoint)));
     */
}

float Scale::min(void) const
{
    return this->mmin;
}

float Scale::max(void) const
{
    return this->mmax;
}

Scale::Placement Scale::placement(void) const
{
    return this->mPlacement;
}

void Scale::setUnitLabel(QString unit)
{
    this->unitLabel = unit;
}

void Scale::translate(float dis)
{
    this->translation = dis;
}

void Scale::scale(float fact)
{
    this->scaleFactor = fact;
}

void Scale::setMin(float min)
{
    this->mmin = (min + this->translation) * this->scaleFactor;
    this->adjust();
}

void Scale::setMax(float max)
{
    this->mmax = (max + this->translation) * this->scaleFactor;
    this->adjust();
}

void Scale::setResolution(float resolution)
{
    if (resolution <= 0.0)
        return;

    this->initResolution = resolution;
    this->adjust();
}

void Scale::setPrecision(int precision)
{
    if (precision > 0)
        this->precision = precision;
}

void Scale::showContextMenu(QPoint pos)
{
    QPoint gpos = mapToGlobal(pos);
    QMenu menu;
    QAction* horizontalAction = new QAction("Horizontal", &menu);
    QAction* verticalAction = new QAction("Vertical", &menu);

    horizontalAction->setCheckable(true);
    horizontalAction->setChecked(indexHorizontal);
    verticalAction->setCheckable(true);
    verticalAction->setChecked(! indexHorizontal);

    menu.addAction(horizontalAction);
    menu.addAction(verticalAction);

    QAction* selected = menu.exec(gpos);
    if (selected != NULL)
    {
        if (selected == horizontalAction)
            indexHorizontal = selected->isChecked();
        else
            indexHorizontal = ! selected->isChecked();
    }

    this->update();
}

void Scale::adjust(void)
{
    int nNum = qRound((mmax - mmin) / initResolution);
    /*Maybe work on a ratio between place and resolution to exploit in a better way space to display optimal number of ticks */

    if (nNum > 50)
    {
        int tmpRes = qFloor((mmax - mmin) / 50);
        int quot = qRound(tmpRes / initResolution);
        resolution = quot * initResolution;
        nNum = qRound((mmax - mmin) / resolution);
    }
    else if (nNum < 10)
    {
        float tmpRes = (mmax - mmin) / 10;
//        int quot = qCeil(initResolution / tmpRes);
//        int quot = qCeil(tmpRes / initResolution);
//        resolution =  initResolution / quot;
        resolution = tmpRes;
        nNum = qRound((mmax - mmin) / resolution);
    }
    else
    {
        resolution = initResolution;
    }

    int tmp = qCeil(mmin / resolution);
    minToDisplay = tmp * resolution;
    numTicks = nNum;

    this->update();
}

void Scale::displayAutoScaledText(QPainter* painter, const QRectF& zone,
                                  int flags, QString txt)
{
    painter->save();
    QFontMetrics metric = painter->fontMetrics();
    float xfactor = zone.width() / metric.width(txt);

    if (xfactor < 0.9)
    {
        QFont f = painter->font();
        f.setPointSizeF(f.pointSizeF() * xfactor);
        painter->setFont(f);
    }

    painter->drawText(zone, flags | Qt::TextDontClip, txt);
    painter->restore();
}
