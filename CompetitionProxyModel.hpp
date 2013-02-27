#ifndef __COMPETITIONPROXYMODEL_HPP__
#define __COMPETITIONPROXYMODEL_HPP__

#include <QtGui>

class CompetitionProxyModel : public QIdentityProxyModel
{
    public:

        CompetitionProxyModel(QObject* parent = 0);
        virtual ~CompetitionProxyModel();

        virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
};

#endif /* __COMPETITIONPROXYMODEL_HPP__ */
