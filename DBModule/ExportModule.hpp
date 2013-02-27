#ifndef __EXPORTMODULE_HPP__
#define __EXPORTMODULE_HPP__

#include <QtSql>
#include <QtGui>

class ExportModule
{
    public:

        ExportModule(void);

        static bool buildSectorOutput(const QString& competition,
                                      const QDir& dirPath = QDir());
};

#endif /* __EXPORTMODULE_HPP__ */
