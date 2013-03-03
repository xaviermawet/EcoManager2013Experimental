#include "CompetitionProxyModel.hpp"

CompetitionProxyModel::CompetitionProxyModel(QObject* parent) :
    QIdentityProxyModel(parent)
{
}

CompetitionProxyModel::~CompetitionProxyModel(void)
{
}

QVariant CompetitionProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role == Qt::DisplayRole)
    {
        int depth = 0;
        QModelIndex parent = proxyIndex.parent();

        while (parent.isValid())
        {
            parent = parent.parent();
            depth++;
        }

        int num;
        switch (depth)
        {
            case 1:
                num = sourceModel()->data(proxyIndex).toInt();
                return QString(QString("Course %1").arg(num));
            case 2:
                if (proxyIndex.column() == 0)
                {
                    num = sourceModel()->data(proxyIndex).toInt();
                    return QString(QString("Tour %1").arg(num));
                }
            default:;
        }
    }

    return sourceModel()->data(proxyIndex, role);
}
