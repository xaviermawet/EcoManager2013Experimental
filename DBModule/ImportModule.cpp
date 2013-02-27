#include "ImportModule.hpp"

ImportModule::ImportModule()
{
    this->configValid = loadConfig();
}

ImportModule::~ImportModule()
{
}

void ImportModule::addRace(Race& race, QDir dirPath)
{
    QDir dir(dirPath);

    if (!checkFolder(&dir))
    {
        succeeded = false;
        return;
    }

    int idRace(this->createRace(race));

    if (idRace == -1)
    {
        succeeded = false;
        return;
    }

    if (!loadGPSData(dir.filePath(gpsFilename), race))
    {
        qDebug() << "loading gps data failed";
        succeeded = false;
        return;
    }

    qDebug() << "loading gps data suceeded";

    if (!loadSpeedData(dir.filePath(speedFilename), race))
    {
        qDebug() << "loading speed data failed";
        succeeded = false;
        return;
    }

    qDebug() << "loading speed data suceeded";

//    if (!loadAccData(dir.filePath(accFilename), race)) {
//        succeeded = false;
//        return;
//    }

    succeeded = true;
}


int ImportModule::createRace(Race& race)
{
    QSqlQuery wheelQuery("select wheel_radius from COMPETITION where name = ?");
    wheelQuery.addBindValue(race.competition());

    if (! wheelQuery.exec() || ! wheelQuery.next())
    {
        qDebug() << wheelQuery.lastError();
        return -1;
    }

    qreal perimeter = wheelQuery.value(0).toDouble();
    race.setWheelPerimeter(perimeter);

    QSqlQuery numQuery(QString("select max(num) from RACE where ref_compet = \"%1\"").arg(race.competition()));
    int numRace = 1;

    if (numQuery.exec() && numQuery.next())
        numRace = numQuery.value(0).toInt() + 1;
    else
        qDebug() << numQuery.lastError();

    QSqlQuery query;
    query.prepare("insert into RACE (num, date, ref_compet) values (?, ?, ?)");
    query.addBindValue(numRace);
    query.addBindValue(race.date());
    query.addBindValue(race.competition());

    if (query.exec())
    {
        int id = query.lastInsertId().toInt();
        race.setId(id);
        return id;
    }
    else
    {
        race.setId(-1);
        errorString = "Creation course : " + query.lastError().text();
        return -1;
    }
}

bool ImportModule::deleteRace(const Race &race)
{
    QSqlQuery query;
    query.prepare("delete from RACE where id = ?");
    query.addBindValue(race.id());

    if (query.exec())
    {
        qDebug() << "delete empty race ok";
        return true;
    }
    else
    {
        qDebug() << "delete empty race ko";
        return false;
    }
}

int ImportModule::createLap(Race& race, int num, const QTime& start, const QTime& end)
{
    QSqlQuery query;
    query.prepare("insert into LAP (num, ref_race, start_time, end_time) values (?, ?, ?, ?)");

    QTime origin(0, 0);
    int startstamp = origin.msecsTo(start);
    int endstamp = origin.msecsTo(end);

    query.addBindValue(num);
    query.addBindValue(race.id());
    query.addBindValue(startstamp);
    query.addBindValue(endstamp);

    race.addLap(start, end);
    int rc = -1;

    if (! query.exec())
    {
        errorString = "Lap creation failed";
    }
    else
    {
        rc = query.lastInsertId().toInt();
    }

    return rc;
}

bool ImportModule::loadConfig()
{
    QSettings settings;

    if (settings.contains("gps/filename"))
    {
        gpsFilename = settings.value("gps/filename").toString();
    }
    else
    {
        gpsFilename = "gps";
        settings.setValue("gps/filename", gpsFilename);
    }

    if (settings.contains("speed/filename"))
    {
        speedFilename = settings.value("speed/filename").toString();
    }
    else
    {
        speedFilename = "speed";
        settings.setValue("speed/filename", speedFilename);
    }

    if (settings.contains("acceleration/filename"))
    {
        accFilename = settings.value("acceleration/filename").toString();
    }
    else
    {
        accFilename = "accel";
        settings.setValue("acceleration/filename", accFilename);
    }

    if (settings.contains("database"))
    {
        dbName = settings.value("database").toString();
    }
    else
    {
        dbName = "ecomotion.db";
        settings.setValue("database", dbName);
    }

    return true;
}

bool ImportModule::checkFolder(const QDir *dir)
{
    QFile file;

    file.setFileName(dir->filePath(gpsFilename));
    if (!file.exists())
    {
        errorString = gpsFilename + " n'existe pas.";
        return false;
    }

    file.setFileName(dir->filePath(speedFilename));
    if (!file.exists())
    {
        errorString = speedFilename + " n'existe pas.";
        return false;
    }

//    file.setFileName(dir->filePath(accFilename));
//    if (!file.exists()) {
//        errorString = accFilename + " n'existe pas.";
//        return false;
//    }

    dbName = dir->filePath(dbName);
    return true;
}

bool ImportModule::launchQuery(QSqlQuery& query)
{
    bool completed = false;
    QSqlDatabase db = QSqlDatabase::database();
    db.driver()->beginTransaction();

    if (! query.execBatch(QSqlQuery::ValuesAsColumns))
    {
        errorString = "Query failed "+ query.lastQuery() + query.lastError().text();
        qDebug() << "query failed";
    }
    else
    {
        completed = db.driver()->commitTransaction();

        if (!completed)
            errorString = "Transaction failed";
    }

    qDebug() << "query completed : " << completed;
    return completed;
}

bool ImportModule::loadGPSData(const QString &path, Race &race)
{
    QFile gpsFile(path);
    if (!gpsFile.open(QIODevice::ReadOnly))
    {
        errorString = "Impossible d'ouvrir " + path;
        return false;
    }

    QTextStream in(&gpsFile);
    QSqlQuery query;
    query.prepare("insert into POSITION (timestamp, longitude, latitude, altitude, eval_speed, ref_lap_race, ref_lap_num) values (?, ?, ?, ?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList longitudes;
    QVariantList latitudes;
    QVariantList altitudes;
    QVariantList speeds;
    QVariantList refRaces;
    QVariantList refNums;
    QVector<GeoCoordinate> coords;

    int frameCount = 0;
    int nbInterval = 0;
    bool inInterval = false;
    QTime startCollectTime;
    QTime endCollectTime;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        GeoCoordinate coord(line);

        if (coord.goodtype() && coord.valid())
        {
            coords << coord;

            if (! inInterval)
            {
                nbInterval ++;
                qDebug() << frameCount;
                inInterval = true;
            }
            frameCount++;
        }
        else
        {
            qDebug() << "frame not valid : " << line;
            if (coord.goodtype())
            {
                frameCount++;
                inInterval = false;
            }
        }

        if (frameCount == 1)
            startCollectTime = coord.time();
        endCollectTime = coord.time();
    }

    qDebug() << "----> " << nbInterval << " intervals";
    qDebug() << "----> " << (coords.size() * 100.0) / frameCount;

    QList< QPair<QTime, QTime> > laps;

    if (nbInterval > 6 || (coords.size() * 100.0 / frameCount) < 65)
    {
        qDebug() << "[!] Detection skipped";
        laps << QPair<QTime, QTime>(startCollectTime, endCollectTime);
    }
    else
    {
        RaceViewer viewer(coords.toList());

        if (viewer.exec() == QDialog::Accepted)
            laps = viewer.laps();
        else
        {
            QMessageBox::information(0, QString("Annulation"), QString("Chargement de la course annul�e"), QMessageBox::Ok);
            deleteRace(race);
            return false;
        }
//        LapDetector ld(&coords);
//        laps = ld.laps();

        if (laps.size() == 0)
        {
            qDebug() << "[!] no laps founds, creating global one.";
            laps << QPair<QTime, QTime>(startCollectTime, endCollectTime);
        }

    }

    int nbcoord = coords.size();
    int raceId = race.id();
    int j = 0;

    for (int i = 0; i < laps.size(); i++)
    {
        qDebug()  << laps[i].first.toString() << " " << laps[i].second.toString();
        QTime start = laps[i].first;
        QTime end = laps[i].second;

        /*
         * Dans le cas ou l'on aurait des donnees gps alteres et ou la detection de tour serait impossible
         * sur cette base, il faudrait tout de meme charger les donnees vitesses et acceleration pour
         * conserver la visualisation brut des donnees

         * Surement de sages paroles. TODO
         */
        createLap(race, i, start, end);

        /* Filtrage de la zone pre-course (zone stand, ...)*/
        while (i == 0 && j < nbcoord && coords[j].time() < start)
        {
//            qDebug() << "skipping " << j;
            j++;
        }

        while (j < nbcoord && coords[j].time() <= end)
        {
            //                qDebug() << coords[j].time() << start << start.msecsTo(coords[j].time());
            timestamps << start.msecsTo(coords[j].time());
            latitudes << coords[j].latitude();
            longitudes << coords[j].longitude();
            altitudes << coords[j].altitude();
            speeds << coords[j].speed();
            refRaces << raceId;
            refNums << i;
            j++;
        }
    }

    query.addBindValue(timestamps);
    query.addBindValue(longitudes);
    query.addBindValue(latitudes);
    query.addBindValue(altitudes);
    query.addBindValue(speeds);
    query.addBindValue(refRaces);
    query.addBindValue(refNums);

    return launchQuery(query);
}


bool ImportModule::loadSpeedData(const QString &path, Race& race)
{
    qDebug() << "loading speed";
    QFile speedFile(path);

    if (!speedFile.open(QIODevice::ReadOnly))
    {
        errorString = "Impossible d'ouvrir le fichier = " + speedFile.fileName();
        return false;
    }

    QSqlQuery query;
    query.prepare("insert into SPEED (timestamp, value, ref_lap_race, ref_lap_num) values (?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList values;
    QVariantList refRaces;
    QVariantList refNums;
    int raceId = race.id();
    int prevNumLap = -2;
    int numLap;

    qint64 prevAbsTime;
    qint64 absTime;
    qint64 origin;

    /* Le module ecrit ses donnees en little-endian */
//    QDataStream in(&speedFile);
//    in.setByteOrder(QDataStream::LittleEndian);
    QTextStream in(&speedFile);

    if (in.atEnd())
        return true;

//    in >> origin;
    origin = in.readLine().toULongLong();
    prevAbsTime = origin;
    QTime lapTimeOrigin(0, 0);

    while (!in.atEnd())
    {
//        in >> absTime;
        absTime = in.readLine().toULongLong();
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(absTime / (1000 * 1000));
        numLap = race.numLap(dt.time());

        if (numLap != prevNumLap)
        {
            origin = absTime;
            prevNumLap = numLap;
            lapTimeOrigin = race.lap(numLap).first;
        }
//        qDebug() << absTime << " - " << origin << " = " << absTime - origin;

        //FIXME
        Q_ASSERT((absTime - prevAbsTime) >= 0);
        qreal value = race.wheelPerimeter() * 3600.0 * 1000 * 1000 / (absTime - prevAbsTime);

        // FIXME : filter max value
        if (!qIsInf(value) && value < 80) {
            timestamps << lapTimeOrigin.msecsTo(dt.time());
            values << value;
            refRaces << raceId;
            refNums << numLap;
        }

        prevAbsTime = absTime;
    }
    query.addBindValue(timestamps);
    query.addBindValue(values);
    query.addBindValue(refRaces);
    query.addBindValue(refNums);

    return launchQuery(query);
}

bool ImportModule::loadAccData(const QString &path, Race& race)
{
    QFile accFile(path);

    if (!accFile.open(QIODevice::ReadOnly))
    {
        errorString = "Impossible d'ouvrir le fichier = " + accFile.fileName();
        return false;
    }

    QSqlQuery query;
    query.prepare("insert into ACCELERATION (timestamp, g_long, g_lat, ref_lap_race, ref_lap_num) values (?, ?, ?, ?, ?)");
    QVariantList timestamps;
    QVariantList glongs;
    QVariantList glats;
    QVariantList refRaces;
    QVariantList refNums;
    QTextStream in(&accFile);
    int raceId = race.id();
    int numLap;
    QTime origin(0, 0);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList params = line.split(';');
        QTime t = QTime::fromString(params.at(0), "hh:mm:ss");
        numLap = race.numLap(t);

        timestamps << origin.msecsTo(t);
        glongs << params.at(1);
        glats << params.at(2);
        refRaces << raceId;
        refNums << numLap;
    }

    query.addBindValue(timestamps);
    query.addBindValue(glongs);
    query.addBindValue(glats);
    query.addBindValue(refRaces);

    return launchQuery(query);
}


QString ImportModule::getErrorString(void) const
{
    return this->errorString;
}

bool ImportModule::createCompetition(const QString &name, float wheel_radius,
                                     const QString &place)
{
    QSqlQuery query;
    query.prepare("insert into COMPETITION (name, wheel_radius, place) values (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(wheel_radius);
    query.addBindValue(place);
    return query.exec();
}

bool ImportModule::importSuceed(void) const
{
    return this->succeeded;
}

bool ImportModule::isConfigValid(void) const
{
    return this->configValid;
}
