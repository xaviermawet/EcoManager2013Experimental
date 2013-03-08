#include "GroupingTreeModel.hpp"

GroupingTreeModel::GroupingTreeModel(QObject *parent, bool alterable) :
    QAbstractItemModel(parent), rootItem(NULL), sourceModel(NULL),
    defaultPolicy(alterable)
{
}

GroupingTreeModel::~GroupingTreeModel()
{
    delete rootItem;
}

QModelIndex GroupingTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

//    TreeItem* parentItem;

//    // correspond à la méthode nodeFromIndex de la page 253 du livre Qt 4 ------
//    if (!parent.isValid())
//        parentItem = rootItem;
//    else
//        parentItem = static_cast<TreeItem*>(parent.internalPointer());
//    // -------------------------------------------------------------------------

    TreeItem* parentItem = this->nodeFromIndex(parent);

    TreeItem* childItem = parentItem->child(row); // peut être NULL

    if (childItem)
        return createIndex(row, column, childItem); // Méthode de QAbstractItemModel
    else
        return QModelIndex();
}

QModelIndex GroupingTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex(); // un index de modèle invalide représente la racine dans un modèle

    TreeItem* childItem = static_cast<TreeItem*>(child.internalPointer());
    TreeItem* parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();
    else
        return createIndex(parentItem->row(), 0, parentItem);
}

int GroupingTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

//    TreeItem* parentItem;

//    if (!parent.isValid())
//        parentItem = rootItem;
//    else
//        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* parentItem = this->nodeFromIndex(parent);
    if (!parentItem)
        return 0;

    /* Le nombre de ligne d'un élément particulier correspond simplement
     * au nombre d'enfants qu'il possède */
    return parentItem->childrenCount();
}

int GroupingTreeModel::columnCount(const QModelIndex &parent) const
{
    TreeItem* parentItem;

    if (! parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->columnCount();
}

Qt::ItemFlags GroupingTreeModel::flags(const QModelIndex &index) const
{
    if (! index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

    if (item->isAlterable())
        flags |= Qt::ItemIsEditable;

    return flags;
}

QVariant GroupingTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

bool GroupingTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole)
    {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        TreeItem* parent = item->parent();
        int col = -1;/* don't forget root item */

        while (parent)
        {
            col ++;
            parent = parent->parent();
        }
        col += index.column();

        QList<int> realRows = item->mapped();
        bool result(true);
        int realCol;

        if (col < orderedCols.size())
            realCol = orderedCols[col];
        else
            return false;

        foreach (int row, realRows)
        {
            QModelIndex realInd = sourceModel->index(row, realCol);
            result &= sourceModel->setData(realInd, value, role);
            result &= sourceModel->submit();
        }

        if (result)
        {
            item->setData(index.column(), value);
            emit dataChanged(index, index);
        }
        else
        {
            sourceModel->revert();
        }

        return result;
    }

    return false;
}

QVariant GroupingTreeModel::data(const QModelIndex &index, int role) const
{
    if (! index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    return item->data(index.column());
}

void GroupingTreeModel::setSourceModel(QAbstractTableModel* model, const QList<int>& grCols)
{
    if (rootItem)
        delete rootItem;

   QList<QVariant> fields;
   int count(model->columnCount());

   qDebug() << "setSourceModel >> Il y a " << QString::number(count) << " colonnes";

   foreach (int i, grCols)
   {
       qDebug() << QString::number(i);
       fields << model->headerData(i, Qt::Horizontal, Qt::DisplayRole);
   }

   for (int i(0); i < count; i++)
   {
       if (!grCols.contains(i))
       {
           qDebug() << QString::number(i);
           fields << model->headerData(i, Qt::Horizontal, Qt::DisplayRole);
       }
   }

   Group root;
   Group* curGroup = &root;
   rootItem = new TreeItem(fields); // La QList<QVariant> values contiendra les noms de tous les headers
   rootItem->setAlterable(defaultPolicy);
   root.item = rootItem;
   int rowCount(model->rowCount());

   for (int i(0); i < rowCount; i++) // pour toutes les lignes
   {
       for (int j(0); j < grCols.size(); j++) // gr cols = les colonnes que l'on veut regrouper
       {
           int gri = grCols[j]; // gri (groupe index) est le véritable index pour la colonne dans le model passé en paramètre
           TreeItem* grItem; // gr = groupItem
           QModelIndex grIndex = model->index(i, gri); // Index de l'élément dans le model passé en paramètre
           QString groupValue = model->data(grIndex).toString(); // récupére la valeur de l'élément à la position (x,y) = (i, gri) --> valeur utilisée pour grouper les éléments du model "tableau" passé en paramètre

           if (curGroup->children.contains(groupValue))
           {
               curGroup = curGroup->children[groupValue];
               grItem = curGroup->item;
               grItem->addMappedRow(grIndex.row());
           }
           else
           {
               QList<QVariant> values;
               values << model->data(grIndex);

               grItem = new TreeItem(values, grIndex.row(), curGroup->item);
               grItem->setAlterable(defaultPolicy);

               Group* newGroup = new Group;
               newGroup->item = grItem;
               curGroup->children[groupValue] = newGroup;
               curGroup->item->appendChild(grItem);
               curGroup = newGroup;
           }
       }

       QList<QVariant> values;

       for (int j(0); j < count; j++)
       {
           if (! grCols.contains(j))
           {
               QModelIndex ind = model->index(i, j);
               values << model->data(ind);
           }
       }

       TreeItem* subItem = new TreeItem(values, i, curGroup->item);
       subItem->setAlterable(defaultPolicy);
       curGroup->item->appendChild(subItem);
       curGroup = &root;
   }

   sourceModel = model;
   orderedCols = grCols;

   for (int i(0); i < count; i++)
   {
       if (! grCols.contains(i))
           orderedCols << i;
   }
}

TreeItem* GroupingTreeModel::nodeFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return this->rootItem;
    return  static_cast<TreeItem*>(index.internalPointer());
}
