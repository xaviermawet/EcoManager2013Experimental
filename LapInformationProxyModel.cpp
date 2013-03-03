#include "LapInformationProxyModel.hpp"

LapInformationProxyModel::LapInformationProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{
}

LapInformationProxyModel::~LapInformationProxyModel(void)
{
}

QVariant LapInformationProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role != Qt::BackgroundColorRole || proxyIndex.row() % 2 == 0)
        return QIdentityProxyModel::data(proxyIndex, role);

    return QColor(220, 220, 220);
}
