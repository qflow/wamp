#ifndef WAMPBASE_H
#define WAMPBASE_H

#include "signalobserver.h"
#include "functor.h"
#include "registration_p.h"
#include "wampattached.h"
#include <QQmlListReference>
#include <QJSValue>
#include <qqml.h>

namespace QFlow{

class SignalObserver;
class WampBase : public QObject
{
    Q_OBJECT
public:
    WampBase(QObject* parent = NULL);
    void registerList(QString uri, QQmlListReference list);
    void registerMethod(QString uri, QObject* obj, QMetaMethod method);
    void registerPropertyGetter(QString uri, QObject* obj, QMetaProperty prop);
    void registerPropertySetter(QString uri, QObject* obj, QMetaProperty prop);
    void registerProperty(QString uri, QObject* obj, QMetaProperty prop);
    template<typename R, typename ... Args>
    void registerProcedure(QString uri, std::function<R(Args...)> f)
    {
        Functor<R, Args...>* functor = new Functor<R, Args...>(f);
        Impl* impl = new FunctorImpl(functor);
        RegistrationPointer reg(new Registration(uri, impl));
        addRegistration(reg);
    }
    void registerProcedure(QString uri, SimpleCallback callback)
    {
        Impl* impl = new SimpleImpl(callback);
        RegistrationPointer reg(new Registration(uri, impl));
        addRegistration(reg);
    }
    static WampAttached *qmlAttachedProperties(QObject *obj);
    static QVariant deserializeMessage(const QString& message);
    static QString serializeMessage(const QVariant& var);
    static QJSValue variantToJS(const QVariant& var);
public Q_SLOTS:
    void registerProcedure(QString uri, QJSValue callback);
    void registerMethod(QString uri, QObject* obj, QString methodName);
    void registerProperty(QString uri, QObject* obj, QString propName);
    void registerObject(QString uri, QObject* obj);
    void registerSignal(QString uri, QObject* obj, QString signalSignature, bool enabled = true);
protected:
    virtual void addRegistration(RegistrationPointer reg) = 0;
    virtual void addSignalObserver(QString uri, SignalObserverPointer observer) = 0;
};
}
//QML_DECLARE_TYPE( QFlow::WampBase )
QML_DECLARE_TYPEINFO( QFlow::WampBase, QML_HAS_ATTACHED_PROPERTIES )
#endif // WAMPBASE_H
