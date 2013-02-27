#ifndef __GROUPPROXYMODEL_HPP__
#define __GROUPPROXYMODEL_HPP__

#include <QtGui>

class GroupProxyModel : public QAbstractProxyModel
{
    public:

        GroupProxyModel(QObject* parent=0);
        virtual ~GroupProxyModel();

        void setGroupColumn(int ind);

        virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
        virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
        virtual QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex &child) const;
        virtual int rowCount(const QModelIndex &parent) const;
        virtual int columnCount(const QModelIndex &parent) const;
        virtual void setSourceModel(QAbstractItemModel *sourceModel);

        virtual bool hasChildren(const QModelIndex &parent) const;

    protected:

        void createMapping();

        int groupId;

        typedef QList<int> Group;
        QList<Group*> groups;
        QHash<QString, Group*> mapping;
        QList<int*> parentId;
};

#endif /* __GROUPPROXYMODEL_HPP__ */
