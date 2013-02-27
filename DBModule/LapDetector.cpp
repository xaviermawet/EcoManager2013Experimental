#include "LapDetector.hpp"

LapDetector::LapDetector(const QVector<GeoCoordinate> *coords) :
    zones(NULL), realAvg(0)
{
    if (this->seed(coords))
        this->process();
     else
        qDebug() << "seed failed.";
}

LapDetector::~LapDetector(void)
{
    if (this->zones != NULL)
    {
        for (int i(0); i < this->zones->size(); i++)
        {
            QVector<Zone*>* subzone = this->zones->at(i);
            for (int j(0); j < subzone->size(); j++)
                delete subzone->at(j);

            delete subzone;
        }

        delete this->zones;
    }
}

QList<QPair<QTime, QTime> > LapDetector::laps(void) const
{
    return this->gaps;
}

bool LapDetector::seed(const QVector<GeoCoordinate>* coords)
{
    if (coords == NULL || coords->size() <= 0)
        return false;

    qreal minx, maxx;
    qreal miny, maxy;

    minx = maxx = coords->at(0).longitude();
    miny = maxy = coords->at(0).latitude();

    foreach(const GeoCoordinate& c, *coords)
    {
        qreal x = c.longitude();
        qreal y = c.latitude();

        if (minx > x)
            minx = x;
        else if (maxx < x)
            maxx = x;

        if (miny > y)
            miny = y;
        else if (maxy < y)
            maxy = y;
    }

    qreal dx = maxx - minx;
    qreal dy = maxy - miny;
    qreal delta = qMin(dx, dy);
    qreal ratio = 100 / delta;

    dx = dx * ratio / 30;
    dy = dy * ratio / 30;

    // FIXME
//    qreal zsize = GeoCoordinate::getDegreeEquivalence(15) * ratio;
    qreal zsize = 0.0002 * ratio;
    qDebug() << "delta : " << delta << 100/delta;
    qDebug() << "dx, dy : " << dx << dy;
    qDebug() << "zsize : "<< zsize;
    this->zones = new QVector< QVector<Zone*>* >;
    int nbVert = qCeil((maxy - miny) * ratio / zsize);
    int nbHor = qCeil((maxx - minx) * ratio / zsize);

    for (int i(0); i < nbHor; i++)
    {
        QVector<Zone*>* zone = new QVector<Zone*>;
        this->zones->append(zone);

        for (int j(0); j < nbVert; j++)
            zone->append(new Zone);
    }

    int index(0);
    DataPoint* prev(NULL);

    foreach(const GeoCoordinate& c, *coords)
    {
        qreal sx = (c.longitude() - minx) * ratio;
        qreal sy = (c.latitude() - miny) * ratio;

//        int x = qFloor(sx / dx);
//        int y = qFloor(sy / dy);
        int x = qFloor(sx / zsize);
        int y = qFloor(sy / zsize);
        Zone* zone = this->zones->at(x)->at(y);

        if (prev == NULL || ! zone->contains(prev))
        {
            QTime tm;
            c.time(tm);

            DataPoint* np = new DataPoint(x, y, tm, index);
            zone->add(np);
            timeline.append(zone);
            prev = np;
            index ++;
        }
    }

    return true;
}

/*
 *           sum ((xi - x0) ^ 2)
 *variance = ------------------- - (m - x0) ^ 2
 *             sum (ni)
 *
 */
void LapDetector::process(void)
{
    QList<Zone*> filledZones;
    qreal average(0);
    QMap<int, int> classes;

    for (int i(0); i < zones->size(); i++)
    {
        int zsize(zones->at(i)->size());

        for (int j(0); j < zsize; j++)
        {
            Zone* pz(zones->at(i)->at(j));
            qreal disp(pz->dispersion());

            if (! qFuzzyCompare(disp, 0.0))
            {
                filledZones.append(pz);
                average += disp;

                int cl = qRound(disp);
                if (classes.contains(cl))
                {
                    qDebug() << "cl : "<< cl << classes[cl];
                    int& val = classes[cl];
                    val++;
                }
                else
                {
                    classes[cl] = 1;
                }
            }
        }
    }

    QList<int> values(classes.values());
    qSort(values);

    int maxValue(values.last());
    QList<int> keys(classes.keys(maxValue));
    qSort(keys);
    qDebug() << "max : " << maxValue << "keys : " << keys;

    int diff(average);
    int i(0);
    bool found(false);

    while (i < keys.size() && ! found)
    {
        int newDiff = qAbs(average - classes[keys.at(i)]);

        if (i == 0 || diff >= newDiff)
        {
            diff = newDiff;
            i++;
        }
        else
        {
            found = true;
        }
    }

    if (found || i == 1)
        realAvg = keys.at(i - 1);
    else
        realAvg = average;

    qDebug() << "realAvg : " << realAvg;
    this->delimLaps();

}

void LapDetector::delimLaps(void)
{
    int num(0);
    int index(0);
    int lastins(-1);
    Zone* z(NULL);

    while (index < timeline.size())
    {
        z = timeline[index];
        int disp = z->interval(num, realAvg, realAvg * 0.05);

        if (disp != -1)
        {
            qreal ratio = qAbs(1 - disp / realAvg);

            //5% de marge entre l'intervalle de la zone et la moyenne des intervalles calculees
            if (ratio < 0.25)
            {
                QTime start, end;
                z->timeGap(num, start, end);
                gaps.append(QPair<QTime, QTime>(start, end));
                qDebug() << num << z << start << end;

                index += qFloor(disp);
                lastins = index;
                num++;
            }
            else
            {
                index ++;
            }
        }
        else
        {
            index ++;
        }
    }

    //Gestion du dernier tour, qui lui n'est pas visible dans les intervalles s'il est inacheve
    if (lastins >= 0 && lastins < timeline.size() - 1)
    {
        Zone* start = timeline[lastins];
        Zone* end = timeline.last();
        gaps.append(QPair<QTime, QTime>(start->lastPoint(), end->lastPoint()));
        num++;
    }

    qDebug() << num << " laps found.";
}
