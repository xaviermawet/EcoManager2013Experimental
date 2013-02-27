#ifndef __SCALE_HPP__
#define __SCALE_HPP__

#include <QtGui>

class Scale : public QWidget
{
    Q_OBJECT

    public:

        typedef enum { Left, Right, Top, Bottom } Placement;

        explicit Scale(QWidget* parent = 0);

        // Getter
        float min(void) const; // Jamais utilisé
        float max(void) const; // Jamais utilisé
        Placement placement(void) const;

        // Setter
        void setUnitLabel(QString unit);
        void translate(float dis); // que veut dire dis ? distance ?
        void scale(float fact);

    public slots:

        void setMin(float min); // Jamais utilisé - Devrait pas être dans SLOTS
        void setMax(float max); // Jamais utilisé - Devrait pas être dans SLOTS
        void setResolution(float resolution); // Jamais utilisé comme SLOT
        void setPrecision(int precision); // Jamais utilisé - Devrait pas être dans SLOTS

        virtual void cursorAt(QPoint pos) = 0; // Une méthode virtuelle pure dans les SLOTS ? C'est une blague ?

    protected slots:

        void showContextMenu(QPoint pos); // A connecter dans le constructeur ?

    protected:

        virtual void paintEvent(QPaintEvent *) = 0;

        void adjust(void);
        void displayAutoScaledText(QPainter*, const QRectF&, int flags, QString txt);

        float mmin;
        float mmax;
        float initResolution;
        float resolution;
        int numTicks;
        int cursor;
        int precision;
        bool indexHorizontal;
        float minToDisplay;
        QString unitLabel;
        Placement mPlacement;
        float translation;
        float scaleFactor;
};

#endif /* __SCALE_HPP__ */
