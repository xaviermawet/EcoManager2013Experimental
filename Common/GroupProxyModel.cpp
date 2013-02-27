#include "GroupProxyModel.hpp"

GroupProxyModel::GroupProxyModel(QObject* parent) :
    QAbstractProxyModel(parent) {
    groupId = 0;
}

GroupProxyModel::~GroupProxyModel() {
    qDeleteAll(groups);
}

void GroupProxyModel::setGroupColumn(int ind) {
    if (ind < sourceModel()->columnCount())
        groupId = ind;
}

QModelIndex GroupProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
    if (! proxyIndex.isValid())
        return QModelIndex();

    if (proxyIndex.model() != this)
        return QModelIndex();

    int srcRow;
    int srcCol;
    QModelIndex parent = proxyIndex.parent();

    if (parent.isValid()) {
        Group* gr = groups[parent.row()];
        srcRow = gr->at(proxyIndex.row());
        srcCol = (proxyIndex.row() <= groupId) ? proxyIndex.row() : proxyIndex.row() + 1;
    } else {
        Group* gr = groups[proxyIndex.row()];
        srcRow = gr->first();
        srcCol = groupId;
    }

    return sourceModel()->index(srcRow, srcCol);
}

QModelIndex GroupProxyModel::mapFromSource(const QModelIndex &sourceIndex) const {
    if (! sourceIndex.isValid())
        return QModelIndex();

    if (sourceIndex.model() != sourceModel())
        return QModelIndex();

    int srcRow = sourceIndex.row();
    int nbGroup = groups.size();
    bool found = false;
    int rowGroup(-1);

    for (int i = 0; i < nbGroup && ! found; i++) {
        if (groups[i]->contains(srcRow)) {
            rowGroup = i;
            found = true;
        }
    }

    Q_ASSERT(found != false);

    int srcCol = sourceIndex.column();
    int row;
    QModelIndex parent;

    if (srcCol != groupId) {
       parent = mapFromSource(sourceModel()->index(rowGroup, groupId));
       row = (srcCol < groupId) ? srcCol : srcCol - 1;
    } else {
        row = rowGroup;
    }

    return index(row, 0, parent);
}

QModelIndex GroupProxyModel::index(int row, int column, const QModelIndex &parent) const {
    if (row < 0 || column < 0)
        return QModelIndex();

    int* id;

    if (parent.isValid()) {
        Group* ownGroup = groups[parent.row()];
        int tmpid = ownGroup->indexOf(row);
        id = parentId[tmpid + 1];
    } else {
        id = parentId[0];
    }

    return createIndex(row, column, id);
}

QModelIndex GroupProxyModel::parent(const QModelIndex &child) const {
    if (! child.isValid())
        return QModelIndex();

    int* srcParent = (int*) child.internalPointer();

    if (srcParent == NULL || *srcParent == -1)
        return QModelIndex();
    else
        return mapFromSource(sourceModel()->index(*srcParent, 0));
}

void GroupProxyModel::setSourceModel(QAbstractItemModel *sourceModel) {
    QAbstractProxyModel::setSourceModel(sourceModel);
    createMapping();
}

bool GroupProxyModel::hasChildren(const QModelIndex &parent) const {
    return rowCount(parent) > 0;
}

void GroupProxyModel::createMapping() {
    qDeleteAll(groups);
    qDeleteAll(parentId);
    groups.clear();
    mapping.clear();
    parentId.clear();

    int count = sourceModel()->rowCount();
    parentId.append(0);

    for (int i = 0; i< count; i++) {
        QModelIndex dataInd = sourceModel()->index(i, groupId);
        QVariant value = sourceModel()->data(dataInd);
        Group* group = mapping[value.toString()];

        if (group == NULL) {
            group = new Group;
            group->append(i);
            groups.append(group);
            mapping[value.toString()] = group;
        } else {
            group->append(i);
        }
        parentId.append(new int(i));
    }
}

int GroupProxyModel::rowCount(const QModelIndex &parent) const {
    if (! parent.isValid()) {
        return groups.size();
    } else if (! parent.parent().isValid()) {
        Group* gr = groups[parent.row()];
        qDebug() << gr->count();
        return gr->count();
    } else {
        return 0;
    }
}

int GroupProxyModel::columnCount(const QModelIndex&) const {
    return 1;
}
