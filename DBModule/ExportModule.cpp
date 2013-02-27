#include "ExportModule.hpp"

ExportModule::ExportModule(void)
{
}

bool ExportModule::buildSectorOutput(const QString& competition,
                                     const QDir& dirPath)
{
    QSqlQuery sectorQuery;
    sectorQuery.prepare("select latitude, longitude, min_speed, max_speed "
                        "from sector, position "
                        "where start_pos <= position.id and end_pos >= position.id "
                        "and sector.ref_compet = ? "
                        "order by sector.num, position.id");
    sectorQuery.addBindValue(competition);

    if (sectorQuery.exec())
    {
        QSettings settings;
        QString outName = settings.value("module/config_file", "sectors").toString();
        QFile outFile(dirPath.filePath(outName));

        if (outFile.open(QFile::WriteOnly))
        {
            QTextStream out(&outFile);

            while (sectorQuery.next())
            {
                out << sectorQuery.value(0).toString() << ",";
                out << sectorQuery.value(1).toString() << ",";
                out << sectorQuery.value(2).toString() << ",";
                out << sectorQuery.value(3).toString() << "\n";
            }

            outFile.close();
            return true;
        }
        else
        {
            qWarning() << outFile.errorString();
        }
    }

    return false;
}
