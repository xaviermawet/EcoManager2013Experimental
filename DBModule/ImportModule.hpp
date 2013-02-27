#ifndef __IMPORTMODULE_HPP__
#define __IMPORTMODULE_HPP__

#include "Race.hpp"
#include "GeoCoordinate.hpp"
#include "LapDetector.hpp"
#include "../RaceViewer.hpp"
#include <QtGui>
#include <QtSql>

class ImportModule
{
    public:

        ImportModule(void);
        ~ImportModule(void);

        bool importSuceed(void) const;
        bool isConfigValid(void) const;
        QString getErrorString(void) const;

        bool createCompetition(const QString& name, float wheel_radius = 0,
                               const QString& place = QString());
        void addRace(Race& race, QDir dir);

    private:

        int createRace(Race &race);
        bool deleteRace(const Race& race);
        int createLap(Race& race, int num, const QTime& start, const QTime& end);
        bool loadConfig();
        bool loadGPSData(const QString& path, Race& race);
        bool loadSpeedData(const QString& path, Race& race);
        bool loadAccData(const QString& path, Race& race);
        bool checkFolder(const QDir* dir);
        bool launchQuery(QSqlQuery& q);

        bool configValid;
        bool succeeded;
        QString gpsFilename;
        QString speedFilename;
        QString accFilename;
        QString dbName;
        QString errorString;
};

#endif /* __IMPORTMODULE_HPP__ */
