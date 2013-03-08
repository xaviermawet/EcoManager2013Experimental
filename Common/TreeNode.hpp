#ifndef __TREENODE_HPP__
#define __TREENODE_HPP__

#include <QtGui>

class TreeNode
{
    public:

        TreeNode(const QVariant& data, TreeNode* parent = 0);
        TreeNode(const QList<QVariant>& data, TreeNode* parent = 0);
        ~TreeNode(void);

        TreeNode* parent(void);
        TreeNode* child(int row);
        QVariant data(int column) const;

        void appendChild(TreeNode* child);
        int  childCount(void) const;
        int  columnCount(void) const;
        int  row(void) const;

    protected:

        QList<TreeNode*> childItems;
        QList<QVariant> itemData;
        TreeNode* parentItem;

        int count;
};

#endif /* __TREENODE_HPP__ */
