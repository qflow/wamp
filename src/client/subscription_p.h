#ifndef SUBSCRIPTION_P_H
#define SUBSCRIPTION_P_H

#include "functor.h"
#include "helper.h"
#include <QUrl>
#include <QJsonArray>
#include <QJSValue>
#include <QQmlListReference>
#include <QMetaMethod>
#include <QJsonObject>
#include <QJSEngine>

namespace QFlow{

class Subscription
{
protected:
    qulonglong _subscriptionId;
    QString _uri;
public:
    Subscription() : _subscriptionId(-1)
    {

    }
    QString uri() const
    {
        return _uri;
    }
    void setSubscriptionId(qulonglong subscriptionId)
    {
        _subscriptionId = subscriptionId;
    }

    Subscription(QString uri) : _uri(uri)
    {

    }
    qulonglong subscriptionId() const
    {
        return _subscriptionId;
    }
    virtual ~Subscription()
    {

    }
    virtual void handle(const QVariantList& args) = 0;
};
typedef QSharedPointer<Subscription> SubscriptionPointer;
class JSSubscription : public Subscription
{
    QJSValue _callback;
public:
    JSSubscription(QString uri, QJSValue val) : Subscription(uri), _callback(val)
    {
        Q_ASSERT(val.isCallable());
    }
    void handle(const QVariantList& args) override
    {
        QJSValueList params;
        Q_FOREACH (QVariant val, args) {
            QByteArray arr = val.toByteArray();
            //QJSValue param = Helper::variantToJS(val, _callback.engine());
            QJSValue param = _callback.engine()->toScriptValue(val);
            params.append(param);
        }
        _callback.call(params);
    }

    ~JSSubscription()
    {

    }
};
class MethodSubscription : public Subscription
{
    QObject* _obj;
    QString _method;
    QMetaMethod _metaMethod;
public:
    MethodSubscription(QString uri, QObject* obj, QString method) : Subscription(uri), _obj(obj), _method(method)
    {
        int index = _obj->metaObject()->indexOfSlot(method.toLatin1());
        _metaMethod = _obj->metaObject()->method(index);
    }
    void handle(const QVariantList& args) override
    {
        QGenericArgument genArgs[10];
        for(int i=0; i<args.count(); i++)
        {
            QGenericArgument genArg(args[i].typeName(), args[i].constData());
            genArgs[i] = genArg;
        }
        _metaMethod.invoke(_obj, Qt::AutoConnection, genArgs[0], genArgs[1], genArgs[2], genArgs[3],
                genArgs[4], genArgs[5], genArgs[6], genArgs[7], genArgs[8], genArgs[9]);
    }
};

class FunctorSubscription : public Subscription
{
    FunctorBase* _functor;
public:
    FunctorSubscription(QString uri, FunctorBase* functor) : Subscription(uri), _functor(functor)
    {

    }
    ~FunctorSubscription()
    {

    }
    void handle(const QVariantList& args) override
    {
        _functor->call(args);
    }

};
}
#endif // SUBSCRIPTION_P_H

