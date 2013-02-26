#ifndef __GEOCOORDINATE_HPP__
#define __GEOCOORDINATE_HPP__

#include <QtGui>

class GeoCoordinate
{
    public:

        GeoCoordinate();
        GeoCoordinate(QString GGAFrame);

        double latitude() const;
        void setLatitude(qreal);

        double longitude() const;
        void setLongitude(qreal);

        double altitude() const;
        QPointF projection() const;

        float speed() const;
        void time(QTime& ) const;
        void time(int& ) const;
        QTime time() const;
        bool valid() const;
        bool goodtype() const;

        static void setSystemPrecision(qreal meter);
        static qreal getSystemPrecision();
        static qreal getDegreeEquivalence(qreal meter);
        static GeoCoordinate fromPlanProjection(qreal longitude, qreal latitude);

    protected:

        static qreal errorDegree;

        double lat;
        double lon;
        double alt;
        float mspeed;
        QTime mtime;
        bool mvalid;
        bool mgoodtype;
        qreal mProjectionRatio;

        void convertToDegree(QString dmm);
        void processGGAFrame(QString gga);
        void processRMCFrame(QString rmc);
        void processVTGFrame(QString vtg);

};

#endif /* __GEOCOORDINATE_HPP__ */
