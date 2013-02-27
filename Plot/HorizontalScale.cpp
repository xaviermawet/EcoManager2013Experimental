#include "HorizontalScale.hpp"

HorizontalScale::HorizontalScale(Scale::Placement placement, QWidget* parent) :
    Scale(parent)
{
    if (placement == Scale::Bottom || placement == Scale::Top)
        mPlacement = placement;
    else
        mPlacement = Scale::Bottom;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //setMinimumWidth(42);
    setMinimumHeight(50);
}

HorizontalScale::~HorizontalScale(void)
{
}

QSize HorizontalScale::sizeHint(void)
{
    return QSize(42, 40);
}

void HorizontalScale::cursorAt(QPoint pos)
{
    cursor = pos.x();
    this->update();
}

void HorizontalScale::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int len = width();
    float incX = float(len) / numTicks;
    float tickOrigin =  float(len) * (minToDisplay - mmin) / (mmax - mmin);
    painter.drawLine(0, 0, len, 0);

    QRectF origin(tickOrigin, 15, 3 * incX, 15);
    painter.drawLine(tickOrigin, 0, tickOrigin, 15);
    displayIndex(&painter, origin, minToDisplay, Qt::AlignLeft);

    for (int i(1); i < numTicks; i++)
    {
        float x = tickOrigin + incX * i;

        if (i % 5 == 0)
        {
            QRectF txtzone(x -  2 * incX, 15, 4 * incX, 15);
            float tick = minToDisplay + i * resolution;

            displayIndex(&painter, txtzone, tick, Qt::AlignHCenter);
            painter.drawLine(x, 0, x, 15);
        }
        else
        {
            painter.drawLine(x, 0, x, 7);
        }
    }

//    float xend = incX * numTicks - 1;
//    QRectF end(xend - 3 * incX, 15, 3 * incX, 15);
//    painter.drawLine(xend, 0, xend, 15);
//    displayIndex(&painter, end, mmax, Qt::AlignRight);

    QRectF zoneLabel(len - 50, 30, 50, 15);
    painter.drawText(zoneLabel, Qt::AlignRight, unitLabel);
}

void HorizontalScale::mouseMoveEvent(QMouseEvent* ev)
{
    if (ev->x() >= 0 && ev->x() <= width())
    {
        int left = qMin(cursor, ev->x());
        int len = qAbs(cursor - ev->x());
        cursor = ev->x();
        update(left - 4, 9, len + 8, 8);
    }
}

void HorizontalScale::displayCursor(QPainter* painter)
{
    QPolygonF cur;
    int h = height();
    cur << QPointF(cursor, 9);
    cur << QPointF(cursor - 4, 17);
    cur << QPointF(cursor + 4, 17);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(Qt::SolidPattern);
    painter->drawPolygon(cur);

    float valCursor =  mmin + ((mmax - mmin) * cursor / width());
    float inc = float(width()) / numTicks;
    QRectF txtzone(cursor - 2 * inc, 17, 4 * inc,  h - 17);

    painter->eraseRect(txtzone);
    displayAutoScaledText(painter, txtzone, Qt::AlignHCenter, QString::number(valCursor, 'f', precision));
    painter->restore();
}

void HorizontalScale::displayIndex(QPainter* painter, const QRectF& zone, float value, int flag)
{
    QString txt = QString::number(value, 'f', precision);

    if (!indexHorizontal)
    {
        painter->save();
        painter->translate(zone.left(), zone.top());
        painter->rotate(90);
        painter->translate(-zone.left(), -zone.top());

        //check existing flags and adapt them
        if ((flag & Qt::AlignRight) != flag)
            displayAutoScaledText(painter, zone, flag | Qt::AlignVCenter | Qt::AlignLeft, txt);

        painter->restore();
    }
    else
    {
        displayAutoScaledText(painter, zone, flag | Qt::AlignBottom, txt);
    }

}
