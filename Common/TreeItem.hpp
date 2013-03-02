/*
 * Un TreeItem contient :
 *      - QList<QVariant>  values   --> Valeurs pour chaque colonne
 *      - QList<TreeItem*> children --> Liste des tous ses enfants
 *
 * Si un TreeItem est le rootItem, il contient la liste des tous les noeds
 * principaux dans "children" et ne possède aucune valeur dans "values".
 * On utilise donc la liste des valeurs pour y sauver le nom de toutes
 * les colonnes
 */

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
        QList<QVariant> values; // Liste des valeurs pour les différentes colonnes. Si c'est le rootItem sa QList<QVariant> values doit être nule
        int count; // Nombre de colonnes. Si un enfant à un plus grand nombre de colonnes que l'élément courant, cette variable vaudra le nombre de colonnes de sont enfant qui en a le plus
        QList<int> mappedRows;
        bool alterable;
};

#endif /* __TREEITEM_HPP__ */
