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
    if (role == Qt::DisplayRole && proxyIndex.column() == 0)
    {
        int depth = 0;
        QModelIndex parent = proxyIndex.parent();

        for (depth = 0; parent.isValid(); depth++)
            parent = parent.parent();

        int num;
        switch (depth) {
        case 0:
            num = this->sourceModel()->data(proxyIndex).toInt();
            return QString("Course " + QString::number(num));
            break;
        case 1:
            num = this->sourceModel()->data(proxyIndex).toInt();
            return QString("Tour " + QString::number(num));
            break;
        default:;
        }
    }
    else if (role == Qt::BackgroundColorRole && proxyIndex.row() % 2 != 0)
    {
        return QColor(220, 220, 220);
    }

    return QIdentityProxyModel::data(proxyIndex, role);







    /*
    if (role != Qt::BackgroundColorRole || proxyIndex.row() % 2 == 0)
        return QIdentityProxyModel::data(proxyIndex, role);

    return QColor(220, 220, 220);
    */
}
