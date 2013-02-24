#ifndef __RESOURCEINSTALLER_HPP__
#define __RESOURCEINSTALLER_HPP__

#include <QtGui>
#include <QtSql>

class ResourceInstaller
{
    public:

        ResourceInstaller(const QString& organizationName,
                          const QString& applicName);

        bool CheckSettings(void) const;
        void InstallSettings(const QDir& destDir = QDir::current(),
                             const QString& dbName = "EcoMotion.db") const;
        void installSettings(const QString& dirPath,
                             const QString& dbName = "EcoMotion.db") const;

    protected:

        void installDatabase(const QString& name) const;
};

#endif /* __RESOURCEINSTALLER_HPP__ */
