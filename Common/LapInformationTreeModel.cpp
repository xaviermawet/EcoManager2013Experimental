#include "LapInformationTreeModel.hpp"

LapInformationTreeModel::LapInformationTreeModel(const QStringList& headers,
                                                 bool editable,
                                                 QObject* parent) :
    QAbstractItemModel(parent), rootItem(NULL), alterable(editable)
 {
     QVector<QVariant> rootData;
     foreach (QString header, headers)
         rootData << header;

     this->rootItem = new TreeElement(rootData);
 }

 LapInformationTreeModel::~LapInformationTreeModel(void)
 {
     delete this->rootItem;
 }

 int LapInformationTreeModel::columnCount(const QModelIndex& parent) const
 {
     Q_UNUSED(parent);

     return rootItem->columnCount();
/*
     TreeElement* parentItem = this->nodeFromIndex(parent);
     if (!parentItem)
         return 0;

     return parentItem->columnCount();
 */
 }

 QVariant LapInformationTreeModel::data(const QModelIndex& index, int role) const
 {
     //qDebug() << "LapInformationTreeModel --> data";

     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole && role != Qt::EditRole)
         return QVariant();

     TreeElement* item = this->nodeFromIndex(index);

     return item->data(index.column());
 }

 Qt::ItemFlags LapInformationTreeModel::flags(const QModelIndex& index) const
 {
     if (!index.isValid())
         return 0;

     // return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

     Qt::ItemFlags flags = Qt::ItemIsEnabled;
     TreeElement* item = this->nodeFromIndex(index);

     // Seuls les noeds sans enfant (feuilles) peuvent etre sélectionnés
     if (item->childCount() <= 0)
         flags |= Qt::ItemIsSelectable;

     // Si le model est modifiable
     if (this->alterable)
         flags |= Qt::ItemIsEditable;

     return flags;
 }

 TreeElement* LapInformationTreeModel::nodeFromIndex(const QModelIndex& index) const
 {
     if (index.isValid())
     {
         TreeElement *item = static_cast<TreeElement*>(index.internalPointer());
         if (item) return item;
     }

     return this->rootItem;
 }
 QVariant LapInformationTreeModel::headerData(int section,
                                              Qt::Orientation orientation,
                                              int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return this->rootItem->data(section);

     return QVariant();
 }

 QModelIndex LapInformationTreeModel::index(int row, int column,
                                            const QModelIndex& parent) const
 {
     /*
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();
     */
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     TreeElement *parentItem = this->nodeFromIndex(parent);

     TreeElement *childItem = parentItem->child(row); // peut être NULL

     if (childItem)
         return this->createIndex(row, column, childItem); // Méthode de QAbstractItemModel
     else
         return QModelIndex();
 }

 bool LapInformationTreeModel::insertColumns(int position, int columns,
                                             const QModelIndex& parent)
 {
     bool success;

     this->beginInsertColumns(parent, position, position + columns - 1);
     success = this->rootItem->insertColumns(position, columns);
     this->endInsertColumns();

     return success;
 }

 bool LapInformationTreeModel::insertRows(int position, int rows,
                                          const QModelIndex &parent)
 {
     TreeElement* parentItem = this->nodeFromIndex(parent);
     bool success;

     this->beginInsertRows(parent, position, position + rows - 1);
     success = parentItem->insertChildren(position, rows, rootItem->columnCount());
     this->endInsertRows();

     return success;
 }

 QModelIndex LapInformationTreeModel::parent(const QModelIndex& child) const
 {
     // un index de modèle invalide représente la racine dans un modèle
     if (!child.isValid())
         return QModelIndex();

     TreeElement *childItem = this->nodeFromIndex(child);
     TreeElement *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);

     /* dans la méthode createIndex, le paramètre column prend la valeur 0 car
      * il n'y a qu'une seule colonne (indice 0) dans notre structure. C'est
      * les nodes qui ont une liste de paramètres pour les différentes colonnes
      * (fictives)
      */
 }

 bool LapInformationTreeModel::removeColumns(int position, int columns,
                                             const QModelIndex& parent)
 {
     bool success;

     this->beginRemoveColumns(parent, position, position + columns - 1);
     success = rootItem->removeColumns(position, columns);
     this->endRemoveColumns();

     /* Si toutes les colonnes ont été supprimées, on supprime également toues
      * les lignes
      */
     if (this->rootItem->columnCount() == 0)
         this->removeRows(0, this->rowCount());

     return success;
 }

 bool LapInformationTreeModel::removeRows(int position, int rows,
                                          const QModelIndex& parent)
 {
     TreeElement* parentItem = this->nodeFromIndex(parent);
     bool success = true;

     this->beginRemoveRows(parent, position, position + rows - 1);
     success = parentItem->removeChildren(position, rows);
     this->endRemoveRows();

     return success;
 }

 int LapInformationTreeModel::rowCount(const QModelIndex& parent) const
 {
     // il ne peut y avoir des enfants que dans le première colonne
     if (parent.column() > 0)
         return 0;

     TreeElement* parentItem = this->nodeFromIndex(parent);
     if (!parentItem)
         return 0;

     /* Le nombre de ligne d'un élément particulier correspond simplement
      * au nombre d'enfants qu'il possède */
     return parentItem->childCount();
 }

 bool LapInformationTreeModel::setData(const QModelIndex &index,
                                       const QVariant& value, int role)
 {
     if (role != Qt::EditRole)
         return false;

     TreeElement* item = this->nodeFromIndex(index);
     bool result = item->setData(index.column(), value);

     if (result)
         emit dataChanged(index, index);

     return result;
 }

 bool LapInformationTreeModel::setHeaderData(int section,
                                             Qt::Orientation orientation,
                                             const QVariant &value, int role)
 {
     if (role != Qt::EditRole || orientation != Qt::Horizontal)
         return false;

     bool result = rootItem->setData(section, value);

     if (result)
         emit headerDataChanged(orientation, section, section);

     return result;
 }

 void LapInformationTreeModel::addRaceInformation2(int refRace, int refLap,
                                                   const QList<QVariant> &data)
 {
     if (!this->insertRow(this->rootItem->childCount()))
         return;
 }

 void LapInformationTreeModel::addRaceInformation(int refRace, int refLap,
                                                  const QList<QVariant> &data)
 {
     TreeElement* refRaceNode = NULL; bool refRaceFound(false);
     TreeElement* refLapNode  = NULL; bool refLapFound(false);

     // Check if an entry for the race already exists
     for (int i(0); i < this->rootItem->childCount(); ++i)
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
         refRaceNode = new TreeElement(refRace, this->rootItem);
         this->rootItem->appendChild(refRaceNode);
     }
     else
     {
         /* Entry for the race exists, check if an entry for the lap ref exist
          * in the race reference entry */
         for (int i(0); i < refRaceNode->childCount(); ++i)
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
         refLapNode = new TreeElement(refLap, refRaceNode);
         refRaceNode->appendChild(refLapNode);
     }

     // Append row with lap information
     TreeElement* newLapInfoNode = new TreeElement(data.toVector(), refLapNode);
     refLapNode->appendChild(newLapInfoNode);

     this->reset();
 }

 void LapInformationTreeModel::addMultipleRaceInformation(
         int refRace, int refLap, const QList<QList<QVariant> > &data)
 {
     TreeElement* refRaceNode = NULL;
     TreeElement* refLapNode  = NULL;

     // Check if an entry for the race already exists
     for (int i(0); i < this->rootItem->childCount(); ++i)
     {
         refRaceNode = this->rootItem->child(i);

         if (refRaceNode->data(0).toInt() == refRace)
             break;
     }

     // Create entry for the race if it dosen't exists
     if (!refRaceNode)
     {
         refRaceNode = new TreeElement(refRace, this->rootItem);
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
         refLapNode = new TreeElement(refLap, refRaceNode);
         refRaceNode->appendChild(refLapNode);
     }

     qDebug() << "addMultipleRaceInformation --> Nombre de lignes : " << data.count();

     // Append rows with lap information
     for (int i(0); i < data.count(); ++i)
     {
         TreeElement* newLapInfoNode = new TreeElement(data.at(i).toVector(),
                                                       refLapNode);
         refLapNode->appendChild(newLapInfoNode);
     }

     this->reset();
 }
