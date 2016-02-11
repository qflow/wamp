#include "wampbase.h"
#include "signalobserver.h"
#include <QQmlProperty>
#include <QJsonDocument>

namespace QFlow{

WampBase::WampBase(QObject* parent) : QObject(parent)
{

}
void WampBase::registerPropertyGetter(QString uri, QObject *obj, QMetaProperty prop)
{
    Impl* impl = new PropertyGetterImpl(obj, prop);
    RegistrationPointer reg(new Registration(uri, impl));
    addRegistration(reg);
}
void WampBase::registerPropertySetter(QString uri, QObject *obj, QMetaProperty prop)
{
    Impl* impl = new PropertySetterImpl(obj, prop);
    RegistrationPointer reg(new Registration(uri, impl));
    addRegistration(reg);
}
void WampBase::registerList(QString uri, QQmlListReference list)
{
    for(int i=0;i<list.count();i++)
    {
        QObject* childObj = list.at(i);
        QString childUri(QString("%1.%2").arg(uri).arg(i));
        registerObject(childUri, childObj);
    }
    QString countUri = uri + ".count";
    Impl* impl = new QmlListCountImpl(list);
    RegistrationPointer reg(new Registration(countUri, impl));
    addRegistration(reg);
}

void WampBase::registerProperty(QString uri, QObject *obj, QMetaProperty prop)
{
    if(QString(prop.name()) == "parent") return;
    QVariant value = prop.read(obj);
    if(value.canConvert<QObject*>())
    {
        registerObject(uri, value.value<QObject*>());
        return;
    }
    QQmlProperty qmlProp(obj, prop.name());
    if(qmlProp.propertyTypeCategory() == QQmlProperty::List)
    {
        QQmlListReference ref(obj, prop.name());
        registerList(uri, ref);
        return;
    }

    if(prop.isReadable())
        registerPropertyGetter(uri, obj, prop);
    if(prop.isWritable())
    {
        QString setUri(uri + ".set");
        registerPropertySetter(setUri, obj, prop);
    }
}
void WampBase::registerProperty(QString uri, QObject *obj, QString propName)
{
    int index = obj->metaObject()->indexOfProperty(propName.toLatin1());
    QMetaProperty prop = obj->metaObject()->property(index);
    registerProperty(uri, obj, prop);
}

void WampBase::registerMethod(QString uri, QObject *obj, QString methodName)
{
    int index = obj->metaObject()->indexOfMethod(methodName.toLatin1());
    QMetaMethod metaMethod = obj->metaObject()->method(index);
    registerMethod(uri, obj, metaMethod);
}
void WampBase::registerMethod(QString uri, QObject* obj, QMetaMethod method)
{
    Impl* impl = new MethodImpl(obj, method);
    RegistrationPointer reg(new Registration(uri, impl));
    addRegistration(reg);
}
void WampBase::registerProcedure(QString uri, QJSValue callback)
{
    Impl* impl = new JSImpl(callback);
    RegistrationPointer reg(new Registration(uri, impl));
    addRegistration(reg);
}
void WampBase::registerSignal(QString uri, QObject *obj, QString signalSignature, bool enabled)
{
    SignalObserver* observer = new SignalObserver(obj, signalSignature.toUtf8(), enabled);
    addSignalObserver(uri, observer);
}

void WampBase::registerObject(QString uri, QObject *obj)
{
    const WampAttached *const attached = qobject_cast<WampAttached*>(
                qmlAttachedPropertiesObject<WampBase>(obj, false));
    if(attached && !attached->isRemote()) return;
    const QMetaObject* meta = obj->metaObject();
    for(int i=0;i<meta->methodCount();i++)
    {
        QMetaMethod method = meta->method(i);
        if(method.access() != QMetaMethod::Public ||
                method.name() == "deleteLater") continue;
        QString methodName = method.name();
        QString methodUri(QString("%1.%2").arg(uri).arg(methodName));
        if(method.methodType() == QMetaMethod::Slot) registerMethod(methodUri, obj, method);
        else if(method.methodType() == QMetaMethod::Signal)
        {
            QString sig = method.methodSignature();
            registerSignal(methodUri, obj, sig, false);
        }
    }
    for(int i=0;i<meta->propertyCount();i++)
    {
        QMetaProperty prop = meta->property(i);
        QString propName = prop.name();
        QString propUri(QString("%1.%2").arg(uri).arg(propName));
        registerProperty(propUri, obj, propName);
    }
}
WampAttached *WampBase::qmlAttachedProperties(QObject *obj)
{
    return new WampAttached(obj);
}
}
