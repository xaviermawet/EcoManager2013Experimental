#ifndef __PLOTCURVE_HPP__
#define __PLOTCURVE_HPP__

#include "../Common/IndexedPosition.hpp"
#include "../Common/CoordinateItem.hpp"
#include "../Map/AnimateSectorItem.hpp"
#include <QtGui>

class PlotCurve : public QGraphicsItem
{
    public:

        PlotCurve(QVariant id = QVariant(), QGraphicsItem* parent = 0);
        PlotCurve(const QList<QPointF>& p, QVariant id = QVariant(),
                  QGraphicsItem* parent = 0);
        PlotCurve(const QList<IndexedPosition>& p, QVariant id = QVariant(),
                  QGraphicsItem* parent = 0);

        void addToCurve(QGraphicsItem *item); // Ajouter un CoordinateItem (point) au tracé
        void setCurveVisible(bool visible); // Modifie le flag visible de toutes les lignes qui composent le tracé
        void setPointsVisible(bool visible); // Modifie le flag visible de tous les points qui composent le tracé
        void setPen(const QPen& p); // Modifie le "pen" de toutes les les lignes et point du tracé
        QVariant id(void) const; // Retourne index (et non l'identifiant) associé au tracé --> tous les points ont le même index

        virtual QRectF boundingRect() const; // Retourne les coordonnées du rectangle dans lequel sont compris tous les points
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        QList<CoordinateItem*> nearestCoordinateitems(float timeValue) const;
        QList<CoordinateItem*> onSector(float minTimeValue, float maxTimeValue) const;
        AnimateSectorItem* sectorOn(float timeValue) const;
        AnimateSectorItem* sectorOn(float minTimeValue, float maxTimeValue) const;

        enum { Type = UserType + 5 };
        int type() const { return Type; }

    protected:

        QList<QGraphicsLineItem*> curve; // toutes les lignes qui composent le tracé sur le graphique --> permet d'afficher seulement des lignes sur le graphique
        QList<QGraphicsItem*> points;    // tous les points qui composent le tracé sur le graphique --> permet d'aficher seulement les points sur le graphique --> il ajoute des CoordinateItem en réallité
        QGraphicsItem* last; // dernier point, utilisé pour raccorder les lignes ensemble au fur et a mesure des ajouts de points qui composent le tracé --> est un CoordinateItem
        QPen pen;
        bool curveVisible;  // Définir si on doit afficher les lignes sur le graphique
        bool pointsVisible; // Définir si on doit afficher les points sur le graphique
        QVariant internalId; // index --> tous les points ont le même index
};

#endif /* __PLOTCURVE_HPP__ */
