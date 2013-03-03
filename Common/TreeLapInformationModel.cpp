#include "TreeLapInformationModel.hpp"

TreeLapInformationModel::TreeLapInformationModel(const QList<QVariant>& headers,
                                                 QObject* parent) :
    QAbstractItemModel(parent)
{
    this->rootItem = new TreeNode(headers);
}

TreeLapInformationModel::~TreeLapInformationModel(void)
{
    delete this->rootItem;
}

QModelIndex TreeLapInformationModel::index(int row, int column,
                                           const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeNode* parentItem = this->nodeFromIndex(parent);

    TreeNode* childItem = parentItem->child(row); // peut être NULL

    if (childItem)
        return createIndex(row, column, childItem); // Méthode de QAbstractItemModel
    else
        return QModelIndex();
}

QModelIndex TreeLapInformationModel::parent(const QModelIndex& child) const
{
     // un index de modèle invalide représente la racine dans un modèle
    if (!child.isValid())
        return QModelIndex();

    TreeNode* childItem = static_cast<TreeNode*>(child.internalPointer());
    TreeNode* parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();
    else
        return createIndex(parentItem->row(), 0, parentItem);

    /* dans la méthode createIndex, le paramètre column prend la valeur 0 car
     * il n'y a qu'une seule colonne dans notre structure. C'est les nodes qui
     * ont une liste de paramètres pour les différentes colonnes (fictives)
     */
}

Qt::ItemFlags TreeLapInformationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    // Création d'un modèle en lecture seule
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int TreeLapInformationModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeNode* parentItem = this->nodeFromIndex(parent);
    if (!parentItem)
        return 0;

    /* Le nombre de ligne d'un élément particulier correspond simplement
     * au nombre d'enfants qu'il possède */
    return parentItem->childCount();
}

int TreeLapInformationModel::columnCount(const QModelIndex& parent) const
{
    TreeNode* parentItem = this->nodeFromIndex(parent);
    if (!parentItem)
        return 0;

    return parentItem->columnCount();
}

QVariant TreeLapInformationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeNode* item = static_cast<TreeNode*>(index.internalPointer());

    return item->data(index.column());
}

QVariant TreeLapInformationModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return this->rootItem->data(section);

    return QVariant();
}

void TreeLapInformationModel::addRaceInformation(const QList<QVariant> &data)
{
    // Add simply a new tree node to the root item
    TreeNode* newNode = new TreeNode(data, this->rootItem);
    this->rootItem->appendChild(newNode);

    this->reset();
}

TreeNode* TreeLapInformationModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return this->rootItem;
    return  static_cast<TreeNode*>(index.internalPointer());
}
