#ifndef __TREEELEMENT_HPP__
#define __TREEELEMENT_HPP__

#include <QtGui>

class TreeElement
{
    public:

        TreeElement(const QVariant& uniqueData, TreeElement* parent = 0);
        TreeElement(const QVector<QVariant>& data, TreeElement* parent = 0);
        ~TreeElement(void);

        TreeElement* parent(void);
        TreeElement* child(int row);
        QVariant data(int column) const;

        int childCount(void) const;
        int columnCount(void) const;

        void appendChild(TreeElement* child);

        bool insertChildren(int position, int count, int columns);
        bool insertColumns(int position, int columns);
        bool removeChildren(int position, int count);
        bool removeColumns(int position, int columns);

        int childNumber(void) const; // Quel est son numéro de ligne chez le parent
        bool setData(int column, const QVariant &value);

    protected:

        QList<TreeElement*> childItems;
        QVector<QVariant> itemData;
        TreeElement* parentItem;
};

#endif /* __TREEELEMENT_HPP__ */
