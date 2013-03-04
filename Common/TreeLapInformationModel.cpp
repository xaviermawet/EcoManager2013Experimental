#include "TreeLapInformationModel.hpp"

TreeLapInformationModel::TreeLapInformationModel(const QList<QVariant>& headers,
                                                 QObject* parent) :
    QAbstractItemModel(parent)
{
    this->rootItem = new TreeNode(headers);
    //this->reset();
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

    /* Création d'un modèle en lecture seule +
     * Seuls les noeds sans enfant peuvent etre sélectionnés
     */
    TreeNode* item = static_cast<TreeNode*>(index.internalPointer());

    if (item->childCount())
        return Qt::ItemIsEnabled;

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

void TreeLapInformationModel::addRaceInformation(int refRace, int refLap,
                                                 const QList<QVariant> &data)
{
    TreeNode* refRaceNode = NULL; bool refRaceFound(false);
    TreeNode* refLapNode  = NULL; bool refLapFound(false);

    // Check if an entry for the race already exists
    for (int i(0); i < this->rootItem->childCount(); i++)
    {
        refRaceNode = this->rootItem->child(i);

        if (refRaceNode->data(0).toInt() == refRace)
        {
            refRaceFound = true;
            break;
        }
    }

    // Create entry for the race if it dosen't exists
    if (!refRaceFound)
    {
        qDebug() << "Création d'une entree pour la course";
        refRaceNode = new TreeNode(refRace, this->rootItem);
        this->rootItem->appendChild(refRaceNode);
    }
    else
    {
        /* Entry for the race exists, check if an entry for the lap ref exist
         * in the race reference entry */
        for (int i(0); i < refRaceNode->childCount(); i++)
        {
            refLapNode = refRaceNode->child(i);

            if (refLapNode->data(0).toInt() == refLap)
            {
                refLapFound = true;
                break;
            }
        }
    }

    if (!refLapFound)
    {
        qDebug() << "Creation d'une entree pour le tour";
        refLapNode = new TreeNode(refLap, refRaceNode);
        refRaceNode->appendChild(refLapNode);
    }

    // Append row with lap information
    TreeNode* newLapInfoNode = new TreeNode(data, refLapNode);
    refLapNode->appendChild(newLapInfoNode);

    this->reset();
}

void TreeLapInformationModel::addMultipleRaceInformation(
        int refRace, int refLap, const QList<QList<QVariant> > &data)
{
    TreeNode* refRaceNode = NULL;
    TreeNode* refLapNode  = NULL;

    // Check if an entry for the race already exists
    for (int i(0); i < this->rootItem->childCount(); i++)
    {
        refRaceNode = this->rootItem->child(i);

        if (refRaceNode->data(0).toInt() == refRace)
            break;
    }

    // Create entry for the race if it dosen't exists
    if (!refRaceNode)
    {
        refRaceNode = new TreeNode(refRace, this->rootItem);
        this->rootItem->appendChild(refRaceNode);
    }
    else
    {
        /* Entry for the race exists, check if an entry for the lap ref exist
         * in the race reference entry */
        for (int i(0); i < refRaceNode->childCount(); i++)
        {
            refLapNode = refRaceNode->child(i);

            if (refLapNode->data(0).toInt() == refLap)
                break;
        }
    }

    if (!refLapNode)
    {
        refLapNode = new TreeNode(refLap, refRaceNode);
        refRaceNode->appendChild(refLapNode);
    }

    // Append rows with lap information
    for (int i(0); i < data.count(); i++)
    {
        TreeNode* newLapInfoNode = new TreeNode(data.at(i), refLapNode);
        refLapNode->appendChild(newLapInfoNode);
    }

    this->reset();
}

TreeNode* TreeLapInformationModel::nodeFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return this->rootItem;
    return  static_cast<TreeNode*>(index.internalPointer());
}
