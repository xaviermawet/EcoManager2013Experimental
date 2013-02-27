#include "TreeItem.hpp"

TreeItem::TreeItem(QVariant value, int realRow, TreeItem *parent) :
    parentItem(parent), count(1), alterable(false)
{
    this->values << value;
    this->mappedRows << realRow;
}

TreeItem::TreeItem(QList<QVariant> rowValues, int realRow, TreeItem *parent) :
    parentItem(parent), values(rowValues), count(values.count()), alterable(false)
{
    this->mappedRows << realRow;
}

TreeItem::~TreeItem(void)
{
    qDeleteAll(children);
    children.clear();
}

TreeItem* TreeItem::parent(void) const
{
    return this->parentItem;
}

TreeItem* TreeItem::child(int row) const
{
    return this->children.value(row);
}

QVariant TreeItem::data(int column) const
{
    return this->values.value(column);
}

bool TreeItem::setData(int column, QVariant value)
{
    bool modified(false);

    if (column < this->values.size())
    {
        this->values[column] = value;
        modified = true;
    }

    return modified;
}

int TreeItem::columnCount(void) const
{
    return this->count;
}

int TreeItem::childrenCount(void) const
{
    return this->children.count();
}

int TreeItem::row(void) const
{
    if (this->parentItem)
        return this->parentItem->children.indexOf(const_cast<TreeItem*>(this));
    else
        return 0;
}

QList<int> TreeItem::mapped(void) const
{
    return this->mappedRows;
}

bool TreeItem::isAlterable(void) const
{
    return this->alterable;
}

void TreeItem::setAlterable(bool b)
{
    this->alterable = b;
}

void TreeItem::appendChild(TreeItem *child)
{
    this->children.append(child);

    if (child->columnCount() > this->count)
        this->count = child->columnCount();
}

void TreeItem::addMappedRow(int realRow)
{
    this->mappedRows << realRow;
}
