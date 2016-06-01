#include "treemodel.h"
#include "treeitem.h"

namespace QFlow{
TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent)
{
    _rootItem = new TreeItem(QVariantList({"Title", "Uri"}));
}
TreeModel::~TreeModel()
{
    delete _rootItem;
}
void TreeModel::setData(WampConnection* connection)
{
    _connection = connection;
    connection->call2(KEY_LIST_REGISTRATION_URIS, {},
                      [this](const QVariant& result){
        beginResetModel();
        QStringList uriList = result.toStringList();
        for(QString uri: uriList) {
            _rootItem->add(uri);
        }
        endResetModel();
        std::function<void(double, QVariantMap)> handler = std::bind(&TreeModel::registrationCreated, this,
                                                                     std::placeholders::_1,
                                                                     std::placeholders::_2);
        _connection->subscribe(KEY_REGISTRATION_ON_CREATE, handler);
    });
}
void TreeModel::registrationCreated(double registrationId, QVariantMap args)
{
    QString uri = args["uri"].toString();
    QList<TreeItem*> itemsAdded = _rootItem->add(uri);
    for(TreeItem* item: itemsAdded) {
        TreeItem* parent = item->parent();
        QModelIndex parentIndex = createIndex(parent->index(), 0, parent);
        int row = item->index();
        beginInsertRows(parentIndex, row, row);
        endInsertRows();
    }
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->index(), 0, parentItem);
}
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    int count = parentItem->childCount();
    return count;
}
int TreeModel::columnCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.isValid())
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    else
        parentItem = _rootItem;
    int count = parentItem->columnCount();
    return count;
}
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if(!item) return QVariant();

    switch (role) {
    case TitleRole:
        return item->dataAt(0);
    case UriRole:
        return item->dataAt(1);
    default:
        return QVariant();
    }
}
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _rootItem->dataAt(section);

    return QVariant();
}
QHash<int, QByteArray> TreeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TreeRoles::TitleRole] = "Title";
    roles[TreeRoles::UriRole] = "Uri";
    return roles;
}
}
