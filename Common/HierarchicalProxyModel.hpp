#ifndef __HIERARCHICALPROXY_HPP__
#define __HIERARCHICALPROXY_HPP__

#include <QtGui>

class HierarchicalProxyModel : public QAbstractProxyModel
{
    public:

        HierarchicalProxyModel(QObject* parent=0);
        virtual ~HierarchicalProxyModel(void);

        virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
        virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
        virtual QModelIndex index(int row, int column,
                                  const QModelIndex& parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex& child) const;
        virtual int rowCount(const QModelIndex& parent) const;
        virtual int columnCount(const QModelIndex& parent) const;
        virtual void setSourceModel(QAbstractItemModel* sourceModel);

        virtual bool hasChildren(const QModelIndex& parent) const;

    protected:

        QMap<int, int*> _parentIds;
};

#endif /* __HIERARCHICALPROXY_HPP__ */
