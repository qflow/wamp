#ifndef WAMPMESSAGESERIALIZER_H
#define WAMPMESSAGESERIALIZER_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

namespace QFlow{

class WampMessageSerializer : public QObject
{
public:
    explicit WampMessageSerializer(QObject* parent = NULL);
    virtual ~WampMessageSerializer();
    virtual QByteArray serialize(const QVariantList& arr) = 0;
    virtual QVariantList deserialize(const QByteArray& message) = 0;
    virtual bool isBinary() const;
    static WampMessageSerializer* create(const QString& name);
};
class JsonMessageSerializer : public WampMessageSerializer
{
public:
    explicit JsonMessageSerializer(QObject* parent = NULL);
    virtual ~JsonMessageSerializer();
    QByteArray serialize(const QVariantList& arr);
    QVariantList deserialize(const QByteArray& message);
};

class MsgpackMessageSerializer : public WampMessageSerializer
{
public:
    explicit MsgpackMessageSerializer(QObject* parent = NULL);
    virtual ~MsgpackMessageSerializer();
    QByteArray serialize(const QVariantList& arr);
    QVariantList deserialize(const QByteArray& message);
    bool isBinary() const Q_DECL_OVERRIDE;
};
}
#endif // WAMPMESSAGESERIALIZER_H
