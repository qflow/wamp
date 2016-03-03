#ifndef REGISTRATION_P_H
#define REGISTRATION_P_H

#include "functor.h"
#include "helper.h"
#include "future.h"
#include <QUrl>
#include <QJsonArray>
#include <QJSValue>
#include <QQmlListReference>
#include <QMetaMethod>
#include <QSharedPointer>
#include <QJSEngine>
#include <QThread>
#include <QDateTime>
#include <QDebug>

namespace QFlow{

class WampResult
{
    Q_GADGET
    Q_PROPERTY(QVariant resultData READ resultData)
    Q_PROPERTY(QString errorUri READ errorUri)
private:
    QVariant _resultData;
    QString _errorUri;
public:
    WampResult(QVariant resultData = QVariant(), QString errorUri = QString()) : _resultData(resultData),
        _errorUri(errorUri)
    {

    }
    WampResult(QString errorUri) : _errorUri(errorUri)
    {

    }

    ~WampResult()
    {

    }
    WampResult(const WampResult& other) : _resultData(other._resultData), _errorUri(other._errorUri)
    {

    }

    QVariant resultData() const
    {
        return _resultData;
    }
    Q_INVOKABLE bool isError() const
    {
        return !(_errorUri.isEmpty() || _errorUri.isNull());
    }
    QString errorUri() const
    {
        return _errorUri;
    }
    Q_INVOKABLE bool isNull() const
    {
        return _resultData.isNull();
    }
};
class Impl
{
public:
    virtual WampResult execute(const QVariantList& args) = 0;
    virtual ~Impl()
    {

    }
};
class Registration : public Impl
{
protected:
    qulonglong _registrationId;
    QString _uri;
    QScopedPointer<Impl> _impl;
public:
    QString uri() const
    {
        return _uri;
    }
    qulonglong registrationId() const
    {
        return _registrationId;
    }
    void setRegistrationId(qulonglong registrationId)
    {
        _registrationId = registrationId;
    }

    Registration(QString uri, Impl* impl) : _uri(uri), _impl(impl)
    {

    }
    virtual ~Registration()
    {

    }
    WampResult execute(const QVariantList& args) override
    {
        return _impl->execute(args);
    }
};
typedef QSharedPointer<Registration> RegistrationPointer;

class JSImpl : public Impl
{
    QJSValue _callback;
public:
    JSImpl(QJSValue val) : _callback(val)
    {

    }

    WampResult execute(const QVariantList& args) override
    {
        QJSValueList params;
        for(QVariant val: args) {
            //QJSValue param = Helper::variantToJS(val, _callback.engine());
            QJSValue param = _callback.engine()->toScriptValue(val);
            params.append(param);
        }
        QJSValue jsResult = _callback.call(params);
        QVariant result = jsResult.toVariant();
        return WampResult(result);
    }

    virtual ~JSImpl()
    {

    }
};
class MethodImpl : public Impl
{
    QObject* _obj;
    QMetaMethod _metaMethod;
public:
    MethodImpl(QObject* obj, QMetaMethod metaMethod) : _obj(obj),
        _metaMethod(metaMethod)
    {

    }
    MethodImpl(QObject* obj, QString method) : _obj(obj)
    {
        int methodIndex = obj->metaObject()->indexOfMethod(method.toLatin1());
        _metaMethod = obj->metaObject()->method(methodIndex);
    }

    virtual ~MethodImpl()
    {

    }

    WampResult execute(const QVariantList& args) override
    {
        QGenericArgument genArgs[10];
        QVariantList translatedArgs = args;
        for(int i=0; i<args.count(); i++)
        {
            if((QMetaType::Type)_metaMethod.parameterType(i) == QMetaType::QDateTime &&
                    (QMetaType::Type)translatedArgs[i].type() == QMetaType::QString)
            {
                translatedArgs[i] = QVariant(QDateTime::fromString(translatedArgs[i].toString(), Qt::ISODate));
            }
            else translatedArgs[i].convert(_metaMethod.parameterType(i));
            genArgs[i] = QGenericArgument(translatedArgs[i].typeName(), translatedArgs[i].constData());
        }

        QVariant retVar;
        if(_metaMethod.returnType() != QMetaType::Void) retVar = QVariant(_metaMethod.returnType(), 0);
        QGenericReturnArgument retArg(_metaMethod.typeName(), retVar.data());

        QThread* currentThread = QThread::currentThread();
        Qt::ConnectionType connectionType = Qt::BlockingQueuedConnection;
        if(currentThread == _obj->thread()) connectionType = Qt::DirectConnection;
        _metaMethod.invoke(_obj, connectionType, retArg,  genArgs[0], genArgs[1], genArgs[2], genArgs[3],
                genArgs[4], genArgs[5], genArgs[6], genArgs[7], genArgs[8], genArgs[9]);

        if(retVar.type() == (QVariant::Type)QMetaType::QVariant)
        {
            QVariant inner = retVar.value<QVariant>();
            retVar = inner;
        }
        return WampResult(retVar);

    }
};
class PropertyGetterImpl : public Impl
{
    QObject* _obj;
    QMetaProperty _metaProperty;
public:
    PropertyGetterImpl(QObject* obj, QMetaProperty metaProperty) : _obj(obj),
        _metaProperty(metaProperty)
    {

    }
    virtual ~PropertyGetterImpl()
    {

    }

    WampResult execute(const QVariantList& /*args*/) override
    {
        QVariant resultVar = _metaProperty.read(_obj);
        return WampResult(resultVar);
    }
};
class QmlListCountImpl : public Impl
{
    QQmlListReference _ref;
public:
    QmlListCountImpl(QQmlListReference ref) : _ref(ref)
    {

    }
    virtual ~QmlListCountImpl()
    {

    }

    WampResult execute(const QVariantList& /*args*/) override
    {
        QVariant resultVar(_ref.count());
        return WampResult(resultVar);
    }
};
class PropertySetterImpl : public Impl
{
    QObject* _obj;
    QMetaProperty _metaProperty;
public:
    PropertySetterImpl(QObject* obj, QMetaProperty metaProperty) : _obj(obj),
        _metaProperty(metaProperty)
    {

    }
    virtual ~PropertySetterImpl()
    {

    }

    WampResult execute(const QVariantList& args) override
    {
        _metaProperty.write(_obj, args[0]);
        return WampResult();
    }
};
class FunctorImpl : public Impl
{
    FunctorBase* _functor;
public:
    FunctorImpl(FunctorBase* functor) : _functor(functor)
    {

    }
    virtual ~FunctorImpl()
    {

    }
    WampResult execute(const QVariantList& args) override
    {
        QVariant resultVar = _functor->call(args);
        return WampResult(resultVar);
    }

};
typedef std::function<WampResult(QVariantList)> SimpleCallback;
class SimpleImpl : public Impl
{
    SimpleCallback _callback;
public:
    SimpleImpl(SimpleCallback callback) : _callback(callback)
    {

    }
    virtual ~SimpleImpl()
    {

    }
    WampResult execute(const QVariantList& args) override
    {
        return _callback(args);
    }
};
}
#endif // REGISTRATION_P_H

