#ifndef __LAPDATACOMPARTOR_HPP__
#define __LAPDATACOMPARTOR_HPP__

#define TIME_MS_COLUMN  1
#define TIME_S_COLUMN   2
#define DISTANCE_COLUMN 3
#define SPEED_COLUMN    4

#include "Common/LapInformationTreeModel.hpp"
#include "LapInformationProxyModel.hpp"
#include <QtGui>

namespace Ui {
class LapDataCompartor;
}

class LapDataCompartor : public QDialog
{
    Q_OBJECT
    
    public:

        explicit LapDataCompartor(int raceNumber, int lapNumber,
                                  QWidget *parent = 0);
        ~LapDataCompartor(void);

        void addLapsData(const QList<QVariant>& dataLap1,
                         const QList<QVariant>& dataLap2);

    private:

        void createTableModel(void);

    protected:

        //virtual void paintEvent(QPaintEvent* event);

    protected:

        // GUI
        Ui::LapDataCompartor *ui;

        int raceNum;
        int lapNum;

        // Personal Model
        LapInformationTreeModel* lapDataTableModel;
};

#endif /* __LAPDATACOMPARTOR_HPP__ */
