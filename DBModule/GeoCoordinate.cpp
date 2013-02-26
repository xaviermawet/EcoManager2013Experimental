#include "GeoCoordinate.hpp"

/* DMS Format expected
 *
 * [d]ddmm.mmmm,{E|W|N|S}
 *
 */
GeoCoordinate::GeoCoordinate() : mtime(0, 0) {
    mspeed = 0;
    alt = 0;
    lon = 0;
    lat = 0;
    mvalid = false;
}

GeoCoordinate::GeoCoordinate(QString frame) : mtime(0, 0) {
    mvalid = true;
    mspeed = 0.0;
    alt = 0.0;

    qlonglong timestamp = frame.section(",", -1, -1).toULongLong() / (1000 * 1000);
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);
    mtime = dt.time();

    if (frame.startsWith("$GPGGA")) {
        processGGAFrame(frame.section(",", 0, -2));
        mgoodtype = true;
    } else if (frame.startsWith("$GPRMC")) {
        processRMCFrame(frame.section(",", 0, -2));
        mgoodtype = true;
    } else {
        mvalid = false;
        mgoodtype = false;
    }

//    qDebug() << GGAFrame.section(",", -1, -1) << timestamp << mtime;
}

void GeoCoordinate::processGGAFrame(QString gga) {
//    if (!gga.startsWith("$GPGGA")) {
//        mvalid = false;
//        mgoodtype = false;
//        return;
//    }

    /*
      O : fix not valid or not available
      1 : fix valid (SPS mode)
      2 : fix valid (DGPS mode)
      3 : fix valid (PPS mode)
      6 : estimated, considered here as not valid
    */
    QString statusString = gga.section(",", 6, 6);
    int status = statusString.toInt();

    if (status == 0 || status == 6) {
        mvalid = false;
        return;
    }

    /*
    QString time = gga.section(",", 1, 1);
    QTime timestamp = QTime::fromString(time, "hhmmss.zzz");
    if (!timestamp.isValid()) {
        qDebug() << "time not valid";
        mvalid = false;
        return;
    }
    mtime = timestamp;
    */

    QString latString = gga.section(",", 2, 3);
    QString longString = gga.section(",", 4, 5);
    this->convertToDegree(longString);
    this->convertToDegree(latString);
    this->setLatitude(lat);
}

void GeoCoordinate::processRMCFrame(QString rmc) {
    QString status = rmc.section(",", 2, 2); // A : Active | V : Void

    if (status.compare("V") == 0) {
        mvalid = false;
        return;
    }

    qDebug() << "rmc status : "<< status;
    QString latString = rmc.section(",", 3, 4);
    QString longString = rmc.section(",", 5, 6);
    this->convertToDegree(longString);
    this->convertToDegree(latString);
    this->setLatitude(lat);
}

void GeoCoordinate::processVTGFrame(QString vtg) {
    if (!vtg.startsWith("$GPVTG")) {
        mvalid = false;
        return;
    }

    mspeed = vtg.section(",", 7, 7).toFloat(); //delta t sur un tour de roue
}

void GeoCoordinate::convertToDegree(QString dmm) {
    QRegExp rx("\\d{4,5}\\.\\d{4},[EWNS]");
    if (!mvalid || !rx.exactMatch(dmm)) {
        qDebug() << dmm << "don't match to degree format";
        mvalid = false;
        return;
    }

    QStringList params = dmm.split(',');
    int deg;
    qreal min;
    QString raw = params.at(0);
    if (params.at(1).contains(QRegExp("[EW]"))) {
       deg = raw.mid(0, 3).toInt();
       min = raw.mid(3).toDouble();
       lon = deg + min / 60.0;
       if (params.at(1).compare("W") == 0)
           lon *= -1;
    } else {
       deg = raw.mid(0, 2).toInt();
       min = raw.mid(2).toDouble();
       lat = deg + min / 60.0;
       if (params.at(1).compare("S") == 0)
           lat *= -1;
    }
    mvalid = true;
}

double GeoCoordinate::latitude() const {
    return lat;
}

void GeoCoordinate::setLatitude(qreal l) {
    if (l >= -90 && l <= 90) {
        lat = l;
        qreal pi_180 = 4 * atan(double(1)) / 180;
        const int earthRadius = 6378137; /*m a l'equateur*/
        qreal degAtEquator = pi_180 * earthRadius;
        qreal degAtThisLat = pi_180 * earthRadius * qCos(pi_180 * lat);
        mProjectionRatio = degAtEquator / degAtThisLat;
    }
}

double GeoCoordinate::altitude() const {
    return alt;
}

QPointF GeoCoordinate::projection() const {
    return QPointF(lon / mProjectionRatio, lat);
}

float GeoCoordinate::speed() const {
    return mspeed;
}

void GeoCoordinate::time(QTime& time) const {
    time = mtime;
}

void GeoCoordinate::time(int& timestamp) const {
    QTime origin(0, 0);
    timestamp = origin.msecsTo(mtime);
}

QTime GeoCoordinate::time() const {
    return mtime;
}

double GeoCoordinate::longitude() const {
    return lon;
}

void GeoCoordinate::setLongitude(qreal l) {
    if (l >= -180 && l <= 180)
        lon = l;
}

bool GeoCoordinate::valid() const {
    return mvalid;
}

bool GeoCoordinate::goodtype() const {
    return mgoodtype;
}

void GeoCoordinate::setSystemPrecision(qreal meter) {
    errorDegree = GeoCoordinate::getDegreeEquivalence(meter);
}

qreal GeoCoordinate::errorDegree = getDegreeEquivalence(15);

qreal GeoCoordinate::getDegreeEquivalence(qreal meter) {
    const int earthRadius = 6378137; /*m*/
    qreal pi = 4 * atan(double(1));
    qreal degreeDelta = (360 * meter) / (2 * pi * earthRadius);
    return degreeDelta;
}

GeoCoordinate GeoCoordinate::fromPlanProjection(qreal longitude, qreal latitude) {
    qreal pi_180 = 4 * atan(double(1)) / 180;
    const int earthRadius = 6378137; /*m a l'equateur*/
    qreal degAtEquator = pi_180 * earthRadius;
    qreal degAtThisLat = pi_180 * earthRadius * qCos(pi_180 * latitude);
    qreal projectionRatio = degAtEquator / degAtThisLat;

    GeoCoordinate tmp;
    tmp.setLatitude(latitude);
    tmp.setLongitude(longitude * projectionRatio);

    return tmp;
}

qreal GeoCoordinate::getSystemPrecision() {
    return errorDegree;
}
