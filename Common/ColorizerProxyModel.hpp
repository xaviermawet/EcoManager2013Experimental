#ifndef __COLORIZERPROXYMODEL_HPP__
#define __COLORIZERPROXYMODEL_HPP__

#include "ColorPicker.hpp"
#include <QIdentityProxyModel>

class ColorizerProxyModel : public QIdentityProxyModel
{
    public:

        ColorizerProxyModel(int numDifferentColor, QObject* parent = 0);

        QVariant data(const QModelIndex& proxyIndex, int role) const;
        void setIndexColumn(int ind);

    private:

        ColorPicker picker;
        int indexColumn;
};

#endif /* __COLORIZERPROXYMODEL_HPP__ */
