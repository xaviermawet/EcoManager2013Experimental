#include "VerticalScale.hpp"

VerticalScale::VerticalScale(Scale::Placement placement, QWidget *parent) :
    Scale(parent)
{
    if (placement == Scale::Right || placement == Scale::Left)
        mPlacement = placement;
    else
        mPlacement = Scale::Left;

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMinimumWidth(50);
}

VerticalScale::~VerticalScale()
{
}

void VerticalScale::cursorAt(QPoint pos)
{
    cursor = pos.y();
    update();
}

void VerticalScale::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int len = height();
    int w = width();

    float incValue = resolution;
    float tickOrigin =  float(len) * (minToDisplay - mmin) / (mmax - mmin);
    float inc = float(len) / numTicks;

    float yorigin = len - tickOrigin - 1;
    QRectF origin;

    switch (mPlacement)
    {
        case Left:
            painter.drawLine(w - 1, 0, w - 1, len); /* main axis */
            origin = QRectF(0, yorigin - inc, w - 12, inc);
            painter.drawLine(w, yorigin, w - 12, yorigin);
            displayIndex(&painter, origin, minToDisplay, Qt::AlignBottom | Qt::AlignRight);
            break;
        case Right:
            painter.drawLine(0, 0, 1, len); /* main axis */
            origin = QRectF(12, yorigin - inc, w - 11, inc);
            painter.drawLine(0, yorigin, 11, yorigin);
            displayIndex(&painter, origin, minToDisplay, Qt::AlignBottom | Qt::AlignRight);
            break;
        default:;
    }

    // Dessine toutes les petites barres horizontales
    for (int i(1); i < numTicks; i++)
    {
        float y = yorigin - inc * i;

        if (i % 5 == 0)
        {
            QRectF txtzone;
            float tick = minToDisplay + i * incValue;

            switch (mPlacement)
            {
                case Left:
                    txtzone = QRectF(0, y - inc/2, w - 12, inc);
                    painter.drawLine(w, y, w - 12, y);
                    displayIndex(&painter, txtzone, tick, Qt::AlignVCenter | Qt::AlignRight);
                    break;
                case Right:
                    txtzone = QRectF(12, y - inc/2, w - 11, inc);
                    painter.drawLine(0, y, 11, y);
                    displayIndex(&painter, txtzone, tick, Qt::AlignVCenter | Qt::AlignLeft);
                    break;
            default:;
            }
        }
        else
        {
            switch (mPlacement)
            {
                case Left:
                    painter.drawLine(w, y, w - 7, y);
                    break;
                case Right:
                    painter.drawLine(0, y, 7, y);
                    break;
            default:;
            }
        }
    }

//    QRectF end(0, 0, w - 12, inc);
//    painter.drawLine(w, 0, w - 12, 0);
//    displayIndex(&painter, end, mmax, Qt::AlignTop | Qt::AlignRight);
//    displayCursor(&painter);

    QRectF zoneLabel(0, 0, 50, 15);
    painter.drawText(zoneLabel, Qt::AlignLeft, unitLabel);
}

void VerticalScale::mouseMoveEvent(QMouseEvent* ev)
{
    if (ev->y() >= 0 && ev->y() <= height()) {
        cursor = ev->y();
        this->update(0, cursor - 6, width(), 12);
    }
}

void VerticalScale::displayCursor(QPainter* painter)
{
    QPolygonF cur;
    int w = width();
    cur << QPointF(w - 9, cursor);
    cur << QPointF(w - 17, cursor - 4);
    cur << QPointF(w - 17, cursor + 4);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(Qt::SolidPattern);
    painter->drawPolygon(cur);

    float valCursor =  mmax - ((mmax - mmin) * cursor / height());
    QRectF txtzone(0, cursor - 6, w - 16, 11);

    painter->eraseRect(txtzone);
    displayAutoScaledText(painter, txtzone, 0, QString::number(valCursor, 'f', precision));
    painter->restore();
}

void VerticalScale::displayIndex(QPainter* painter, const QRectF& zone,
                                 float value, int flag)
{
    painter->save();
    QString txt = QString::number(value, 'f', precision);

    if (! indexHorizontal)
    {
        painter->translate(zone.left(), zone.top());
        painter->rotate(90);
        painter->translate(-zone.left(), -zone.top());
        displayAutoScaledText(painter, zone, flag, txt);
    }
    else
    {
        displayAutoScaledText(painter, zone, flag, txt);
    }

    painter->restore();
}
