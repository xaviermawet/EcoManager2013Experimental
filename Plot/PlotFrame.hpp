#ifndef __PLOTFRAME_HPP__
#define __PLOTFRAME_HPP__

#include "PlotScene.hpp"
#include "PlotView.hpp"
#include "VerticalScale.hpp"
#include "HorizontalScale.hpp"
#include <QtGui>

namespace Ui {
class PlotFrame;
}

class PlotFrame : public QFrame
{
    Q_OBJECT
    
    public:

        explicit PlotFrame(QWidget *parent = 0);
        ~PlotFrame(void);

        // Getter
        PlotScene* scene(void) const;

        void addVerticalAxis(VerticalScale* scale);
        void addHorizontalAxis(HorizontalScale* scale);

    signals:

        void clear(void);
        void minVChanged(float);
        void maxVChanged(float);
        void minHChanged(float);
        void maxHChanged(float);
        void selectionChanged(void);

    private slots:

        // Autoconnect
        void on_eraseToolButton_clicked(void);
        void on_showLineToolButton_toggled(bool checked);

        // Personal slots
        void adaptScales(const QRectF& newRect);

    protected:

        // GUI
        Ui::PlotFrame* ui;
        QVBoxLayout* topScaleLayout;
        QVBoxLayout* bottomScaleLayout;
        QHBoxLayout* leftScaleLayout;
        QHBoxLayout* rightScaleLayout;

        PlotScene* plotScene;
        PlotView*  plotView;
};

#endif /* __PLOTFRAME_HPP__ */
