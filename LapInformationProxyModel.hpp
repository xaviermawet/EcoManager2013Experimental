#ifndef __LAPINFORMATIONPROXYMODEL_HPP__
#define __LAPINFORMATIONPROXYMODEL_HPP__

#include <QtGui>

class LapInformationProxyModel : public QIdentityProxyModel
{
    public:

        LapInformationProxyModel(QObject* parent = 0);
        virtual ~LapInformationProxyModel(void);

        virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
};

#endif /* __LAPINFORMATIONPROXYMODEL_HPP__ */
