#include "TreeNode.hpp"

TreeNode::TreeNode(const QVariant &data, TreeNode *parent) :
    parentItem(parent), count(1)
{
    this->itemData << data;
}

TreeNode::TreeNode(const QList<QVariant>& data, TreeNode* parent) :
    itemData(data), parentItem(parent), count(data.count())
 {
     //this->itemData = data;
 }

 TreeNode::~TreeNode(void)
 {
     qDeleteAll(this->childItems);
     this->childItems.clear();
 }

 TreeNode* TreeNode::parent(void)
 {
     return this->parentItem;
 }

 TreeNode* TreeNode::child(int row)
 {
     return this->childItems.value(row);
 }

 QVariant TreeNode::data(int column) const
 {
     return this->itemData.value(column);
 }

 void TreeNode::appendChild(TreeNode* item)
 {
     this->childItems.append(item);

     if (item->columnCount() > this->count)
         this->count = item->columnCount();
 }

 int TreeNode::childCount(void) const
 {
     return this->childItems.count();
 }

 int TreeNode::columnCount(void) const
 {
     //return itemData.count();
     return this->count;
 }

 int TreeNode::row(void) const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<TreeNode*>(this));
     return 0;
 }
