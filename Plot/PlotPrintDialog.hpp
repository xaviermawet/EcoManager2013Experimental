#ifndef __PLOTPRINTDIALOG_HPP__
#define __PLOTPRINTDIALOG_HPP__

#define INDEX_PAPER   0
#define INDEX_PICTURE 1
#define INDEX_PDF     2

#include <QtGui>

namespace Ui {
class PlotPrintDialog;
}

class PlotPrintDialog : public QDialog
{
    Q_OBJECT

    public:

        typedef enum { printing, picture, pdf} printType;

        explicit PlotPrintDialog(QWidget* parent = 0);
        ~PlotPrintDialog(void);

        // Main options
        printType getPrintType(void) const;
        bool printWithScales(void) const;
        bool printCurrentView(void) const;
        bool printEntireView(void) const;

        // Picture option
        int pictureQuality(void) const;

        // Printer options
        QPrinter::PrinterMode printerMode(void) const;
        QPrinter::PaperSize   paperSize(void) const;
        QPrinter::Orientation paperOrientation(void) const;

    private slots:

        void on_withScaleCheckBox_stateChanged(int state);
        void on_printTypeComboBox_currentIndexChanged(int index);

    protected:

        Ui::PlotPrintDialog* ui;
};

#endif /* __PLOTPRINTDIALOG_HPP__ */
