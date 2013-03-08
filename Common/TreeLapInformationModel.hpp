#ifndef __TREELAPINFORMATIONMODEL_HPP__
#define __TREELAPINFORMATIONMODEL_HPP__

#include "TreeNode.hpp"
#include <QtGui>

class TreeLapInformationModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        TreeLapInformationModel(const QStringList &headers, QObject* parent = 0);
        virtual ~TreeLapInformationModel(void);

        virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
        virtual QModelIndex parent(const QModelIndex &child) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

        virtual int rowCount(const QModelIndex &parent) const;
        virtual int columnCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const;

        void addRaceInformation(int refRace, int refLap,
                                const QList<QVariant>& data);
        void addMultipleRaceInformation(int refRace, int refLap,
                                        const QList< QList<QVariant> >& data);

    private:

        TreeNode* nodeFromIndex(const QModelIndex& index) const;

    protected:

        TreeNode* rootItem;
};

#endif /* __TREELAPINFORMATIONMODEL_HPP__ */
