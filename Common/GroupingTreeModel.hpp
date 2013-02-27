#ifndef __GROUPINGTREEMODEL_HPP__
#define __GROUPINGTREEMODEL_HPP__

#include "TreeItem.hpp"
#include <QtGui>

class GroupingTreeModel : public QAbstractItemModel
{
    public:

        GroupingTreeModel(QObject* parent, bool alterable = false);
        virtual ~GroupingTreeModel();

        virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
        virtual QModelIndex parent(const QModelIndex& child) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex& parent) const;
        virtual int columnCount(const QModelIndex& parent) const;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        virtual bool setData(const QModelIndex& index, const QVariant& value, int role);

        void setSourceModel(QAbstractTableModel* model, const QList<int>& grCols);

    private:

        typedef struct group
        {
            TreeItem* item;
            QHash<QString, struct group*> children;
        } Group;

    protected:

        TreeItem* rootItem;
        QAbstractTableModel* sourceModel;
        QList<int> orderedCols;
        bool defaultPolicy;
};

#endif /* __GROUPINGTREEMODEL_HPP__ */
