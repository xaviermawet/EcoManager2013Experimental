#include "ResourceInstaller.hpp"

ResourceInstaller::ResourceInstaller(const QString& organizationName,
                                     const QString& applicName)
{
    /* These values are used by the QSettings class when it is constructed using
     * the empty constructor. This saves having to repeat this information each
     * time a QSettings object is created.
     */

    QCoreApplication::setOrganizationName(organizationName);
    QCoreApplication::setApplicationName(applicName);
}

/* Check if a database already exists. Otherwise the database is created and
 * some settings are applyed
 */
bool ResourceInstaller::CheckSettings(void) const
{
    QSettings settings;

    if (settings.contains("database"))
    {
        QString dbPath = settings.value("database").toString();

        if (QFile::exists(dbPath)) {
            qDebug() << "[INSTALL] retrieving existing db : " + dbPath;
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(dbPath);
            db.open();

            db.driver()->beginTransaction();
            db.exec("PRAGMA foreign_keys=ON");
            db.exec("PRAGMA journal_mode=MEMORY");
            db.exec("PRAGMA synchronous=OFF");
            db.driver()->commitTransaction();
            return true;
        }
    }

    return false;
}

void ResourceInstaller::InstallSettings(const QDir& destDir,
                                        const QString& dbName) const
{
    QSettings settings;
    QString db = destDir.filePath(dbName);

    settings.setValue("database", db);
    this->installDatabase(db);
}

void ResourceInstaller::installSettings(const QString& dirPath,
                                        const QString& dbName) const
{
    QDir destDir(dirPath);
    this->InstallSettings(destDir, dbName);
}

void ResourceInstaller::installDatabase(const QString &name) const
{
    qDebug() << "[INSTALL] adding db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(name);

    if (db.open())
    {
        db.driver()->beginTransaction();
        db.exec("PRAGMA foreign_keys=on");
        db.exec("PRAGMA journal_mode=MEMORY");
        db.exec("PRAGMA synchronous=OFF");
        db.exec("create table COMPETITION ( name VARCHAR(80) PRIMARY KEY, place VARCHAR(80), wheel_radius FLOAT)");
        db.exec("create table SECTOR ( id INTEGER PRIMARY KEY AUTOINCREMENT, num INTEGER, ref_compet VARCHAR, min_speed REAL DEFAULT 0 CHECK(min_speed <= max_speed), max_speed REAL DEFAULT 0, start_pos INTEGER, end_pos INTEGER, FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name), FOREIGN KEY (start_pos) REFERENCES POSITION(id), FOREIGN KEY (end_pos) REFERENCES POSITION(id))");
        db.exec("create table RACE ( id INTEGER PRIMARY KEY AUTOINCREMENT, num INTEGER, date DATETIME, ref_compet VARCHAR(80), FOREIGN KEY (ref_compet) REFERENCES COMPETITION(name))");
        db.exec("create table LAP ( num INTEGER, start_time TIME, end_time TIME, distance FLOAT, ref_race INTEGER, FOREIGN KEY (ref_race) REFERENCES RACE(id), PRIMARY KEY (num, ref_race))");
        db.exec("create table SPEED ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, value FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race))");
        db.exec("create table ACCELERATION ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, g_long FLOAT, g_lat FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race))");
        db.exec("create table POSITION ( id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp TIME, latitude FLOAT, longitude FLOAT, altitude FLOAT, eval_speed FLOAT, ref_lap_num INTEGER, ref_lap_race  INTEGER, FOREIGN KEY (ref_lap_num, ref_lap_race) REFERENCES LAP(num, ref_race))");
        db.driver()->commitTransaction();
    }
    else
    {
        qFatal("Unable to create the database");
    }
}
