#include "realm.h"
#include "realm_p.h"
#include "wamprouter_p.h"
#include "authenticator.h"
#include "random.h"

namespace QFlow{

RealmPrivate::RealmPrivate()
{

}
RealmPrivate::~RealmPrivate()
{

}

Realm::Realm(QObject *parent) : WampBase(parent), d_ptr(new RealmPrivate())
{
    registerMethod(KEY_LIST_REGISTRATION_URIS, this, "registeredUris()");
    registerMethod("wamp.registration.list", this, "registrationIds()");
    registerMethod("wamp.registration.list_internal_uris", this, "registeredInternalUris()");

    registerMethod("wamp.subscription.count_subscribers", this, "subscribersCount(QString)");
    registerProcedure(KEY_GET_SUBSCRIPTION, [this](QVariantList args){//register some object
        qulonglong subscriptionId = (qulonglong)args[0].toDouble();
        if(!this->d_ptr->_subscriptions.contains(subscriptionId))
        {
            return WampResult(QVariant("wamp.error.no_such_subscription"));
        }
        WampRouterSubscriptionPointer subscription = this->d_ptr->_subscriptions[subscriptionId];
        QVariantList resultArr;
        QVariantMap details;
        details["id"] = (double)subscription->subscriptionId();
        details["created"] = subscription->created().toString("yyyy-mm-ddThh:mm:zzzZ");
        details["uri"] = subscription->topic();
        resultArr.append(details);
        return WampResult(QVariant(resultArr));
    });
    registerProcedure(KEY_LOOKUP_REGISTRATION, [this](QVariantList args){
        QString uri = args[0].toString();
        if(!this->d_ptr->_uriRegistartion.contains(uri)) return WampResult();
        WampRouterRegistrationPointer registration = this->d_ptr->_uriRegistartion[uri];
        return WampResult(QVariant(registration->registrationId()));
    });
}
Realm::~Realm()
{

}
QString Realm::name() const
{
    Q_D(const Realm);
    return d->_name;
}
void Realm::setName(QString value)
{
    Q_D(Realm);
    d->_name = value;
}
QQmlListProperty<Role> Realm::roles()
{
    Q_D(Realm);
    return QQmlListProperty<Role>(this, d->_roles);
}
QQmlListProperty<Authenticator> Realm::authenticators()
{
    Q_D(Realm);
    return QQmlListProperty<Authenticator>(this, d->_authenticators);
}
Authenticator* Realm::findAuthenticatorByAuthMethod(QString authMethod) const
{
    Q_D(const Realm);
    for(Authenticator* auth: d->_authenticators)
    {
        if(auth->authMethod() == authMethod) return auth;
    }
    return NULL;
}
bool Realm::containsRegistration(QString uri)
{
    Q_D(Realm);
    QMutexLocker lock(&d->_mutex);
    return d->_uriRegistartion.contains(uri);
}
QStringList Realm::registeredUris()
{
    Q_D(Realm);
    QMutexLocker lock(&d->_mutex);
    return d->_uriRegistartion.keys();
}
QStringList Realm::registeredInternalUris()
{
    Q_D(Realm);
    QMutexLocker lock(&d->_mutex);
    return d->_internalRegistrations.keys();
}
QVariantList Realm::registrationIds()
{
    Q_D(Realm);
    QVariantList res;
    QMutexLocker lock(&d->_mutex);
    for(qulonglong id: d->_idRegistartion.keys())
    {
        res << id;
    }
    return res;
}
void Realm::addRegistration(RegistrationPointer reg)
{
    Q_D(Realm);
    d->_internalRegistrations.insert(reg->uri(), reg);
}
void Realm::addSignalObserver(QString /*uri*/, SignalObserver* /*observer*/)
{

}

qulonglong RealmPrivate::publish(QString topic, const QVariantList& args)
{
    qulonglong publicationId = Random::generate();
    if(_uriSubscriptions.contains(topic))
    {
        QList<WampRouterSubscriptionPointer> subscriptions = _uriSubscriptions.values(topic);
        for(WampRouterSubscriptionPointer subscription: subscriptions)
        {
            subscription->event(publicationId, args);
        }
    }
    return publicationId;
}
qulonglong Realm::publish(QString topic, const QVariantList &args)
{
    Q_D(Realm);
    return d->publish(topic, args);
}

int Realm::subscribersCount(QString topicUri)
{
    Q_D(Realm);
    QMutexLocker lock(&d->_mutex);
    return d->_uriSubscriptions.count(topicUri);
}
void RealmPrivate::insertRegistration(QString uri, WampRouterRegistrationPointer registration)
{
    QMutexLocker lock(&_mutex);
    _uriRegistartion[uri] = registration;
    _idRegistartion.insert(registration->registrationId(), registration);
}
WampRouterRegistrationPointer RealmPrivate::getRegistration(qulonglong registrationId)
{
    QMutexLocker lock(&_mutex);
    return _idRegistartion[registrationId];
}
WampRouterRegistrationPointer RealmPrivate::getRegistration(QString uri)
{
    QMutexLocker lock(&_mutex);
    return _uriRegistartion[uri];
}

void RealmPrivate::removeRegistration(WampRouterRegistrationPointer reg)
{
    QMutexLocker lock(&_mutex);
    _uriRegistartion.remove(reg->uri());
    _idRegistartion.remove(reg->registrationId());
    publish(KEY_REGISTRATION_ON_DELETE, {reg->callee()->sessionId(), reg->registrationId()});
}
void RealmPrivate::insertPendingInvocation(qulonglong requestId, WampRouterSession *session)
{
    QMutexLocker lock(&_mutex);
    _pendingInvocations.insert(requestId, session);
}
bool RealmPrivate::containsInternalRegistration(QString uri)
{
    QMutexLocker lock(&_mutex);
    return _internalRegistrations.contains(uri);
}
RegistrationPointer RealmPrivate::getInternalRegistration(QString uri)
{
    QMutexLocker lock(&_mutex);
    return _internalRegistrations[uri];
}
WampRouterSession* RealmPrivate::takePendingInvocation(qulonglong requestId)
{
    QMutexLocker lock(&_mutex);
    return _pendingInvocations.take(requestId);
}
void RealmPrivate::insertSubscription(qulonglong subscriptionId, QString topic, WampRouterSubscriptionPointer subscription)
{
    QMutexLocker lock(&_mutex);
    _subscriptions.insert(subscriptionId, subscription);
    _uriSubscriptions.insertMulti(topic, subscription);
}
bool RealmPrivate::containsSubscription(QString topic)
{
    QMutexLocker lock(&_mutex);
    return _uriSubscriptions.contains(topic);
}
bool RealmPrivate::containsSubscription(qulonglong subscriptionId)
{
    QMutexLocker lock(&_mutex);
    return _subscriptions.contains(subscriptionId);
}
WampRouterSubscriptionPointer RealmPrivate::takeSubscription(qulonglong subscriptionId)
{
    QMutexLocker lock(&_mutex);
    WampRouterSubscriptionPointer sub = _subscriptions.take(subscriptionId);
    if(!sub)
    {
        qDebug() << QString("Subscription id %1 already removed from realm %2").arg(subscriptionId).arg(_name);
        return WampRouterSubscriptionPointer();
    }
    _uriSubscriptions.remove(sub->topic(), sub);
    if(!_uriSubscriptions.contains(sub->topic()))
    {
        publish(KEY_SUBSCRIPTION_ON_DELETE, {sub->subscriber()->sessionId(), sub->subscriptionId(), sub->topic()});
    }
    publish(KEY_SUBSCRIPTION_ON_UNSUBSCRIBE, {sub->subscriber()->sessionId(), sub->subscriptionId(), sub->topic()});
    return sub;
}
}
