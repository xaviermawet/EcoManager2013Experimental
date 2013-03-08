#include "TreeElement.hpp"

TreeElement::TreeElement(const QVariant& uniqueData, TreeElement* parent) :
    parentItem(parent)
 {
    this->itemData << uniqueData;
 }

TreeElement::TreeElement(const QVector<QVariant>& data, TreeElement* parent) :
    itemData(data), parentItem(parent)
 {
 }

 TreeElement::~TreeElement()
 {
     // it merely calls delete on the items from the container
     qDeleteAll(childItems);
     this->childItems.clear();
 }

 TreeElement* TreeElement::child(int row)
 {
     return this->childItems.value(row);
 }

 int TreeElement::childCount(void) const
 {
     return this->childItems.count();
 }

 int TreeElement::childNumber(void) const
 {
     if (this->parentItem)
         return this->parentItem->childItems.indexOf(
                     const_cast<TreeElement*>(this));
     return 0;
 }

 int TreeElement::columnCount(void) const
 {
     return this->itemData.count();
 }

 void TreeElement::appendChild(TreeElement* child)
 {
     this->childItems.append(child);
 }

 QVariant TreeElement::data(int column) const
 {
     return this->itemData.value(column);
 }

 bool TreeElement::insertChildren(int position, int count, int columns)
 {
     if (position < 0 || position > this->childItems.size())
         return false;

     for (int row(0); row < count; ++row)
     {
         QVector<QVariant> data(columns);
         TreeElement *item = new TreeElement(data, this);
         childItems.insert(position, item);
     }

     return true;
 }

 bool TreeElement::insertColumns(int position, int columns)
 {
     if (position < 0 || position > this->itemData.size())
         return false;

     for (int column(0); column < columns; ++column)
         this->itemData.insert(position, QVariant());

     foreach (TreeElement *child, childItems)
         child->insertColumns(position, columns);

     return true;
 }

 TreeElement *TreeElement::parent(void)
 {
     return this->parentItem;
 }

 bool TreeElement::removeChildren(int position, int count)
 {
     if (position < 0 || position + count > this->childItems.size())
         return false;

     for (int row = 0; row < count; ++row)
         delete this->childItems.takeAt(position);

     return true;
 }

 bool TreeElement::removeColumns(int position, int columns)
 {
     if (position < 0 || position + columns > this->itemData.size())
         return false;

     for (int column = 0; column < columns; ++column)
         this->itemData.remove(position);

     foreach (TreeElement *child, childItems)
         child->removeColumns(position, columns);

     return true;
 }

 bool TreeElement::setData(int column, const QVariant &value)
 {
     if (column < 0 || column >= this->itemData.size())
         return false;

     this->itemData[column] = value;
     return true;
 }
