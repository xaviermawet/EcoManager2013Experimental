#include "ColorizerProxyModel.hpp"

ColorizerProxyModel::ColorizerProxyModel(int numDifferentColor, QObject *parent)
    : QIdentityProxyModel(parent), picker(numDifferentColor), indexColumn(-1)
{
}

QVariant ColorizerProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
    if (role != Qt::BackgroundRole)
        return QIdentityProxyModel::data(proxyIndex, role);

    if (this->indexColumn >= 0 && this->indexColumn < sourceModel()->columnCount())
        return picker.color(sourceModel()->data(sourceModel()->index(proxyIndex.row(), indexColumn)).toInt(), 150);
    else
        return picker.color(proxyIndex.row(), 150);
}

void ColorizerProxyModel::setIndexColumn(int ind)
{
    this->indexColumn = ind;
}
