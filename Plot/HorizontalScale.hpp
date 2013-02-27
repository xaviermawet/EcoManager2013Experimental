#ifndef __HORIZONTALSCALE_HPP__
#define __HORIZONTALSCALE_HPP__

#include "Scale.hpp"
#include <QtGui>

class HorizontalScale : public Scale
{
    Q_OBJECT

    public:

        HorizontalScale(Scale::Placement placement = Scale::Bottom,
                        QWidget* parent = 0);
        virtual ~HorizontalScale(void);

        virtual QSize sizeHint(void);

    public slots:

        virtual void cursorAt(QPoint pos);

    protected:

        virtual void paintEvent(QPaintEvent *);
        virtual void mouseMoveEvent(QMouseEvent *);
        void displayCursor(QPainter*);
        void displayIndex(QPainter*, const QRectF&, float value, int flag = 0);
};

#endif /* __HORIZONTALSCALE_HPP__ */
