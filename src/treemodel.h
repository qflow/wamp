#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "wampconnection.h"
#include <QAbstractItemModel>

namespace QFlow{

class WampConnection;
class TreeItem;
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum TreeRoles {
        TitleRole = Qt::UserRole + 10,
        UriRole = Qt::UserRole + 11
    };
    Q_ENUM(TreeRoles)
    explicit TreeModel(QObject *parent = NULL);
    ~TreeModel();
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
public Q_SLOTS:
    void setData(WampConnection* connection);
private:
    void registrationCreated(double subscriptionId, QVariantMap args);
    WampConnection* _connection;
    TreeItem* _rootItem;
};
}
#endif // TREEMODEL_H
