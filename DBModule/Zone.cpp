#include "Zone.hpp"

Zone::Zone(void) :
    disp(0.0), sumDiff(0)
{
}

Zone::~Zone(void)
{
    /*
    while (!points.empty())
        delete points.takeFirst();
    */

    qDeleteAll(points);
    points.clear();
}

void Zone::add(DataPoint* dp)
{
    /* Si on a seulement un point dans une zone, il ne faut pas laisser son
     * index jouer en valeur absolue
     */
    if (points.size() < 1)
    {
        points.append(dp);
    }
    else
    {
        int diff = dp->index() - points.last()->index();

        qDebug() << "[" << diff << "]";

        diffs.append(diff);
        points.append(dp);
        sumDiff += diff;
        disp = sumDiff / diffs.size();
    }
}

bool Zone::contains(DataPoint* dp) const
{
    return this->points.contains(dp);
}

qreal Zone::dispersion(void) const
{
    return this->disp;
}

void Zone::timeGap(int num, QTime& start, QTime& end) const
{
    if (num >= 0 && num < diffs.size())
    {
        start = points.at(num)->timestamp();
        end   = points.at(num + 1)->timestamp();
    }
    else
    {
        start = QTime();
        end   = QTime();
    }
}

int Zone::interval(int num, qreal width, qreal margin)
{
    if (num >= 0 && num < diffs.size())
    {
        qreal diff = diffs[num];

        if (diff == -1)
            return -1;

        int quot = qRound(diff / width);
        qreal remaining = qAbs(diff - quot * width);

        if (remaining < margin)
        {
            if (quot == 1)
            {
                return diffs[num];
            }
            else if (quot > 0)
            {
                expanse(width);
                return diffs[num];
            }
        }
    }

    return -1;
}

QTime Zone::lastPoint(void) const
{
    if (!points.empty())
        return points.last()->timestamp();
    else
        return QTime();
}

int Zone::gapCount(void) const
{
    return this->diffs.count();
}

void Zone::display(void) const
{
    QTextStream out(stdout);
    out << "[";

    for (int i(0); i < diffs.size(); i++)
        out << " [ " << points.at(i + 1)->index() << " - " << points.at(i)->index() << " ]";

    out << " = " << disp << "]\n";
}

void Zone::expanse(qreal width)
{
    for (int i(0); i < diffs.count(); i++)
    {
        int quot = qFloor(diffs[i] / width);

        if (quot > 0)
            diffs[i] = -1;

        for (int j(1); j < quot; j++)
            diffs.insert(i + j, -1);
    }
}
