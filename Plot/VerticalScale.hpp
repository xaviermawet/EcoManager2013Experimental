#ifndef __VERTICALSCALE_HPP__
#define __VERTICALSCALE_HPP__

#include "Scale.hpp"
#include <QtGui>

class VerticalScale : public Scale
{
    Q_OBJECT

    public:

        VerticalScale(Scale::Placement placement = Left, QWidget* parent=0);
        virtual ~VerticalScale();

    public slots:

        virtual void cursorAt(QPoint pos);

    protected:

        virtual void paintEvent(QPaintEvent *);
        virtual void mouseMoveEvent(QMouseEvent *);

        void displayCursor(QPainter*);
        void displayIndex(QPainter*, const QRectF&, float val, int flag = 0);
};

#endif /* __VERTICALSCALE_HPP__ */
