#include "HierarchicalProxyModel.hpp"

HierarchicalProxyModel::HierarchicalProxyModel(QObject* parent) :
    QAbstractProxyModel(parent)
{
}

HierarchicalProxyModel::~HierarchicalProxyModel(void)
{
}

QModelIndex HierarchicalProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();

    if (sourceIndex.model() != sourceModel())
        return QModelIndex();

    int row;
    int col(0);
    QModelIndex parent;

    if (sourceIndex.column() > 0)
    {
        row = sourceIndex.column() - 1;
        parent = mapFromSource(sourceModel()->index(sourceIndex.row(), 0));
    }
    else
    {
        row = sourceIndex.row();
        parent = QModelIndex();
    }

    return index(row, col, parent);
}

QModelIndex HierarchicalProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    if (proxyIndex.model() != this)
        return QModelIndex();

    int row;
    int col;

    if (proxyIndex.parent().isValid())
    {
        row = proxyIndex.parent().row();
        col = proxyIndex.row() + 1;
    }
    else
    {
        row = proxyIndex.row();
        col = proxyIndex.column();
    }

    return sourceModel()->index(row, col);
}

QModelIndex HierarchicalProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0)
        return QModelIndex();

    QModelIndex srcParent = mapToSource(parent);
    QMap<int, int*>::const_iterator it;

    if (srcParent.isValid())
        it = _parentIds.constFind(srcParent.row());
    else
        it = _parentIds.constFind(-1);

    return createIndex(row, column, *it);
}

QModelIndex HierarchicalProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    int* srcParent = (int*) child.internalPointer();
    QModelIndex srcParentIndex;

    if (*srcParent == -1)
        srcParentIndex = QModelIndex();
    else
        srcParentIndex = sourceModel()->index(*srcParent, 0);

    QModelIndex tmp = mapFromSource(srcParentIndex);
    return tmp;
}

int HierarchicalProxyModel::rowCount(const QModelIndex &parent) const
{
    if (! parent.isValid())
        return sourceModel()->rowCount();

    QModelIndex srcParent = mapToSource(parent);
    if (srcParent.column() > 0)
        return 0;

    return sourceModel()->columnCount() - 1;
}

int HierarchicalProxyModel::columnCount(const QModelIndex&) const
{
    return 1;
}

void HierarchicalProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    int rcount = sourceModel->rowCount();

    for (int i = -1; i < rcount; i++)
        _parentIds[i] = new int(i);

    QAbstractProxyModel::setSourceModel(sourceModel);
}

bool HierarchicalProxyModel::hasChildren(const QModelIndex &parent) const
{
    if (! parent.isValid())
        return true;

    QModelIndex srcParent = mapToSource(parent);

    if (srcParent.column() == 0)
        return true;
    else
        return false;
}
