#ifndef __LAPINFORMATIONTREEMODEL_HPP__
#define __LAPINFORMATIONTREEMODEL_HPP__

#include "TreeElement.hpp"
#include <QtGui>

class LapInformationTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        LapInformationTreeModel(const QStringList& headers,
                                bool editable = false, QObject* parent = 0);
        ~LapInformationTreeModel(void);

        QModelIndex index(int row, int column,
                          const QModelIndex& parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex& child) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;

        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data(const QModelIndex& index,
                      int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;

        bool setData(const QModelIndex &index, const QVariant& value,
                     int role = Qt::EditRole);
        bool setHeaderData(int section, Qt::Orientation orientation,
                           const QVariant& value, int role = Qt::EditRole);


        bool insertColumns(int position, int columns,
                           const QModelIndex& parent = QModelIndex());
        bool removeColumns(int position, int columns,
                           const QModelIndex& parent = QModelIndex());
        bool insertRows(int position, int rows,
                        const QModelIndex& parent = QModelIndex());
        bool removeRows(int position, int rows,
                        const QModelIndex& parent = QModelIndex());

        void addRaceInformation(int refRace, int refLap,
                                const QList<QVariant>& data);
        void addMultipleRaceInformation(int refRace, int refLap,
                                        const QList< QList<QVariant> >& data);

        QVector<QVariant> rowData(const QModelIndex& index) const;

    private:

        TreeElement* nodeFromIndex(const QModelIndex& index) const;

    protected:

        TreeElement* rootItem;
        bool alterable;
};

#endif /* __LAPINFORMATIONTREEMODEL_HPP__ */
