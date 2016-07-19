#include "wampconnection.h"
#include "wampconnection_p.h"
#include "wampattached.h"
#include "future.h"
#include "wampworker.h"
#include "random.h"
#include "wampinvocation.h"
#include "signalobserver.h"
#include "wampmessageserializer.h"
#include "websocketconnection.h"
#include "call.h"
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlProperty>
#include <QMessageAuthenticationCode>

namespace QFlow{

WampConnectionPrivate::WampConnectionPrivate(WampConnection* parent) : QObject(), q_ptr(parent)
{
    _worker = new WampWorker();
    _worker->_socketPrivate = this;
    connect(&_workerThread, &QThread::finished, _worker, &QObject::deleteLater);
    _workerThread.start();
    _worker->moveToThread(&_workerThread);
}
WampConnectionPrivate::~WampConnectionPrivate()
{
    QVariantList arr{(int)WampMsgCode::GOODBYE, QVariantMap(), "wamp.error.system_shutdown"};
    sendWampMessage(arr);
    QMetaObject::invokeMethod(_worker, "flush", Qt::BlockingQueuedConnection);
    //_socket->close();
    _workerThread.quit();
    _workerThread.wait();
}
QByteArray WampConnectionPrivate::IntToOctet(int i)
{
    QByteArray octet;
    octet[0] = (char)((uint)i >> 24);
    octet[1] = (char)((uint)i >> 16);
    octet[2] = (char)((uint)i >> 8);
    octet[3] = (char)i;
    return octet;
}

QByteArray WampConnectionPrivate::PBKDF2(QString password, QString salt, int iterations)
{
    QList<QByteArray> U;
    QByteArray newSalt = salt.toLatin1().append(IntToOctet(1));
    QMessageAuthenticationCode hash(QCryptographicHash::Sha256);
    for(int i=0;i<iterations;i++)
    {
        hash.reset();
        hash.setKey(password.toLatin1());
        hash.addData(newSalt);
        QByteArray hashResult = hash.result();
        U.append(hashResult);
        newSalt = hashResult;
    }
    QByteArray result;
    for(int i=0;i<32;i++)
    {
        char xored = 0;
        for(int j=0;j<iterations;j++)
        {
            char c = U[j][i];
            xored = xored ^ c;
        }
        result[i] = xored;
    }
    return result;
}

void WampConnectionPrivate::handleInvocation(WampInvocationPointer invocation)
{
    WampResult result = invocation->registration->execute(invocation->args);
    QVariant val = result.resultData();
    QVariantList resultArr{val};
    QVariantList arr{(int)WampMsgCode::YIELD, invocation->requestId, QVariantMap()};
    if(val.isValid()) arr.append(QVariant(resultArr));
    sendWampMessage(arr);
}
void WampConnectionPrivate::sendWampMessage(const QVariantList &arr)
{
    if(!_serializer)
    {
        qDebug() << "Serializer not instatiated yet";
        return;
    }
    QByteArray message = _serializer->serialize(arr);
    if(_serializer->isBinary())
    {
        _worker->sendBinaryMessage(message);
    }
    else
    {
       _worker->sendTextMessage(message);
    }
}

void WampConnectionPrivate::handleEvent(const Event& event)
{
    event.subscription->handle(event.args);
}
void WampConnectionPrivate::onConnected()
{
    Q_Q(WampConnection);;

    q->subscribe(KEY_SUBSCRIPTION_ON_CREATE, std::function<void(double, QVariantMap)>{
                     [q, this](double, QVariantMap info){
                         QString topicUri = info["uri"].toString();
                         Q_EMIT q->subscriptionCreated(topicUri);
                         if(!_topicObserver.contains(topicUri)) return;
                         SignalObserverPointer so = _topicObserver[topicUri];
                         so->setEnabled(true);
                     }});
    q->subscribe(KEY_SUBSCRIPTION_ON_DELETE, std::function<void(double, double, QString)>{
                     [q, this](double, double, QString topic){
                         Q_EMIT q->subscriptionDeleted(topic);
                         if(!_topicObserver.contains(topic)) return;
                         SignalObserverPointer so = _topicObserver[topic];
                         so->setEnabled(false);
                     }});
    Q_EMIT q->connected();
}

WampConnection::WampConnection(QObject* parent) : WampBase(parent), d_ptr(new WampConnectionPrivate(this))
{

}
WampConnection::~WampConnection()
{

}
QUrl WampConnection::url() const
{
    Q_D(const WampConnection);
    return d->_url;
}
void WampConnection::setUrl(QUrl value)
{
    Q_D(WampConnection);
    d->_url = value;
    Q_EMIT urlChanged();
}
QString WampConnection::realm() const
{
    Q_D(const WampConnection);
    return d->_realm;
}
void WampConnection::setRealm(QString realm)
{
    Q_D(WampConnection);
    d->_realm = realm;
    Q_EMIT realmChanged();
}
User* WampConnection::user() const
{
    Q_D(const WampConnection);
    return d->_user;
}
void WampConnection::setUser(User* value)
{
    Q_D(WampConnection);
    d->_user = value;
    Q_EMIT userChanged();
}

void WampConnection::connect()
{
    Q_D(WampConnection);
    QMetaObject::invokeMethod(d->_worker, "connect", Qt::QueuedConnection);
}

void WampConnectionPrivate::addRegistration(RegistrationPointer reg)
{
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::REGISTER, requestId, QVariantMap(), reg->uri()};
    _pendingRegistrations[requestId] = reg;
    sendWampMessage(arr);
}
void WampConnectionPrivate::addSubscription(SubscriptionPointer sub)
{
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::SUBSCRIBE, requestId, QVariantMap(), sub->uri()};
    _pendingSubscriptions[requestId] = sub;
    sendWampMessage(arr);
}
void WampConnection::addRegistration(RegistrationPointer reg)
{
    Q_D(WampConnection);
    d->addRegistration(reg);
}
void WampConnection::addSubscription(SubscriptionPointer sub)
{
    Q_D(WampConnection);
    d->addSubscription(sub);
}
void WampConnection::unregister(qulonglong registrationId)
{
    Q_D(WampConnection);
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::UNREGISTER, requestId, registrationId};

    RegistrationPointer reg = d->_registrations.take(registrationId);
    d->_pendingUnregistrations[requestId] = reg;
    d->sendWampMessage(arr);
}
void WampConnection::unsubscribe(qulonglong subscriptionId)
{
    Q_D(WampConnection);
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::UNSUBSCRIBE, requestId, subscriptionId};

    SubscriptionPointer sub = d->_subscriptions.take(subscriptionId);
    d->_pendingUnsubscriptions[requestId] = sub;
    d->sendWampMessage(arr);
}
void WampConnection::subscribe(QString uri, QJSValue callback)
{
    Q_D(WampConnection);
    SubscriptionPointer sub(new JSSubscription(uri, callback));
    d->addSubscription(sub);
}
void WampConnection::subscribe(QString uri, QObject *obj, QString method)
{
    Q_D(WampConnection);
    SubscriptionPointer sub(new MethodSubscription(uri, obj, method));
    d->addSubscription(sub);
}
void WampConnectionPrivate::call(QString uri, const QVariantList &args, CallPointer call)
{
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::CALL, requestId, QVariantMap(), uri, args};
    _pendingCalls[requestId] = call;
    sendWampMessage(arr);
}

Future WampConnection::call(QString uri, const QVariantList& args, const QJSValue& callback)
{
    Q_D(WampConnection);
    Impl* impl = NULL;
    if(callback.isCallable()) impl = new JSImpl(callback);
    CallPointer call(new Call(impl, this), CallDeleter());
    d->call(uri, args, call);
    return call->getFuture();
}
Future WampConnection::call(QString uri, const QVariantList &args, QObject *callbackObj, QString callbackMethod)
{
    Q_D(WampConnection);
    Impl* impl = new MethodImpl(callbackObj, callbackMethod);
    CallPointer call(new Call(impl), CallDeleter());
    d->call(uri, args, call);
    return call->getFuture();
}

Future WampConnection::call2(QString uri, const QVariantList& args, ResultCallback callback)
{
    Q_D(WampConnection);
    Impl* impl = NULL;
    if(callback)
    {
        auto* functor = new Functor<void,QVariant>(callback);
        impl = new FunctorImpl(functor);
    }
    CallPointer call(new Call(impl), CallDeleter());
    d->call(uri, args, call);
    return call->getFuture();
}

void WampConnection::publish(QString uri, const QVariantList& args)
{
    Q_D(WampConnection);
    qulonglong requestId = Random::generate();
    QVariantList arr{(int)WampMsgCode::PUBLISH, requestId, QVariantMap(), uri, args};
    d->sendWampMessage(arr);
}
void WampConnection::define(QString uri, QString definition)
{
    call2(KEY_DEFINE_SCHEMA, {uri, definition});
}
Future WampConnection::describe(QString uri)
{
    return call2(KEY_DESCRIBE_SCHEMA, {uri});
}
Future WampConnection::lookupRegistration(QString uri)
{
    return call2(KEY_LOOKUP_REGISTRATION, {uri});
}
Future WampConnection::listRegistrations()
{
    QVariantMap match;
    match["match"] = "prefix";
    return call2("wamp.registration.list", QVariantList());
}
Future WampConnection::getSubscription(qulonglong subscriptionId)
{
    return call2(KEY_GET_SUBSCRIPTION, {subscriptionId});
}
Future WampConnection::subscribersCount(QString topicUri, ResultCallback callback)
{
    return call2(KEY_COUNT_SUBSCRIBERS, {topicUri}, callback);
}

void WampConnection::unregister(QString uri)
{
    Q_D(WampConnection);
    if(!d->_uriRegistration.contains(uri))
    {
        qWarning() << QString("Cannot unregister not existing registration %1").arg(uri);
        return;
    }
    qulonglong registrationId = d->_uriRegistration[uri]->registrationId();
    unregister(registrationId);
}
void WampConnection::unsubscribe(QString uri)
{
    Q_D(WampConnection);
    if(!d->_uriSubscription.contains(uri))
    {
        qWarning() << QString("Cannot unsubscribe from not existing subscription %1").arg(uri);
        return;
    }
    qulonglong subscriptionId = d->_uriSubscription[uri]->subscriptionId();
    unsubscribe(subscriptionId);
}
void WampConnection::addSignalObserver(QString uri, SignalObserverPointer observer)
{
    Q_D(WampConnection);
    if(d->_topicObserver.contains(uri))
    {
        int i=0;
    }
    d->_topicObserver[uri] = observer;
    subscribersCount(uri, [observer](const QVariant& result){
        int count = result.toInt();
        observer->setEnabled(count > 0);
    });
    QObject::connect(observer.get(), &SignalObserver::signalEmitted, [this, uri](QVariantList args){
        publish(uri, args);
    });
}
}
