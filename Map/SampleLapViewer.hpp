#ifndef __SAMPLELAPVIEWER_HPP__
#define __SAMPLELAPVIEWER_HPP__

#include "MapView.hpp"
#include <QtGui>
#include <QtSql>

namespace Ui {
class SampleLapViewer;
}

class SampleLapViewer : public QDialog
{
    Q_OBJECT
    
    public:

        explicit SampleLapViewer(QWidget* parent = 0);
        ~SampleLapViewer();

        virtual QSize sizeHint(void);
        QPair<int, int> selectedReferencesLap(void) const;

    private slots:

        // Autoconnect
        void on_previousLapButton_clicked(void);
        void on_nextLapButton_clicked(void);
        void on_buttonBox_accepted(void);
        void on_buttonBox_rejected(void);

        // Personal slots
        void zoomView(int increment);

    private:

        MapView* createLapView(int refRace, int refLap);
        void init(void);

    protected:

        Ui::SampleLapViewer* ui;

        int zoomLevel;
        QString refCompetition;
        QList< QPair<int, int> > lapIds;
};

#endif /* __SAMPLELAPVIEWER_HPP__ */
