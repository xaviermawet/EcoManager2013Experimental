#ifndef __TREEITEM_HPP__
#define __TREEITEM_HPP__

#include <QtGui>

class TreeItem
{
    public:

        TreeItem(QVariant value, int realRow = -1, TreeItem* parent = 0);
        TreeItem(QList<QVariant> rowValues, int realRow = -1, TreeItem* parent = 0);
        ~TreeItem(void);

        TreeItem* parent(void) const;
        TreeItem* child(int row) const;
        QVariant data(int column) const;

        bool setData(int column, QVariant value);
        int columnCount(void) const;
        int childrenCount(void) const;
        int row(void) const;
        QList<int> mapped(void) const;

        bool isAlterable(void) const;
        void setAlterable(bool);

        void appendChild(TreeItem* child);
        void addMappedRow(int realRow);

    protected:

        TreeItem* parentItem;
        QList<TreeItem*> children;
        QList<QVariant> values;
        int count;
        QList<int> mappedRows;
        bool alterable;
};

#endif /* __TREEITEM_HPP__ */
