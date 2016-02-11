#include "wamproutersession.h"
#include "wamproutersession_p.h"
#include "realm.h"
#include "realm_p.h"
#include "wamprouter_p.h"
#include "authenticator.h"
#include "user.h"
#include "wamprouterworker.h"
#include "wampmessageserializer.h"
#include "websocketconnection.h"
#include "random.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPointer>
#include <QHostAddress>

namespace QFlow{

WampRouterSessionPrivate::WampRouterSessionPrivate(WampRouterSession* parent) : QObject(), q_ptr(parent)
{
}
WampRouterSessionPrivate::~WampRouterSessionPrivate()
{
    _socket->deleteLater();
}
void WampRouterSessionPrivate::sendWampMessage(const QVariantList& arr)
{
    QByteArray message = _serializer->serialize(arr);
    if(_serializer->isBinary())
    {
        _socket->sendBinary(message);
    }
    else
    {
        _socket->sendText(message);
    }
}
WampRouterSession::WampRouterSession(WebSocketConnection *socket, QString subprotocol, QObject *parent) : QThread(parent), d_ptr(new WampRouterSessionPrivate(this))
{
    Q_D(WampRouterSession);
    d->moveToThread(this);
    d->_sessionId = Random::generate();
    d->_serializer.reset(WampMessageSerializer::create(subprotocol));
    d->_socket = socket;
    QObject::connect(socket, SIGNAL(messageReceived(QByteArray)), d, SLOT(onMessageReceived(QByteArray)));
    QObject::connect(socket, SIGNAL(closed()), d, SLOT(closed()));
    d->_router = (WampRouterWorker*)parent;
    start();
}
void WampRouterSessionPrivate::onMessageReceived(const QByteArray &message)
{
    Q_Q(WampRouterSession);
    QVariantList arr = _serializer->deserialize(message);
    WampMsgCode code = (WampMsgCode)arr[0].toInt();
    if(code == WampMsgCode::HELLO)
    {
        QString realmName = arr[1].toString();
        Realm* realmFound = NULL;
        Q_FOREACH (Realm* r, _router->_realms) {
            if(realmName == r->name())
            {
                realmFound = r;
                break;
            }
        }
        if(!realmFound)
        {
            abort(KEY_ERR_NO_SUCH_REALM, "The realm does not exist.");
            return;
        }
        _realm = realmFound;
        QVariantMap details = arr[2].toMap();
        if(!realmFound->d_ptr->_authenticators.isEmpty())
        {
            QVariantList authMethods2 = details["authmethods"].toList();
            Authenticator* foundAuth = NULL;
            Q_FOREACH(QVariant authMethod, authMethods2)
            {
                foundAuth = realmFound->findAuthenticatorByAuthMethod(authMethod.toString());
                if(foundAuth) break;
            }
            if(foundAuth)
            {
                QString authId = details["authid"].toString();
                QVariantMap challenge = foundAuth->generateChallenge(_sessionId, authId);
                QVariantList authArr{WampMsgCode::CHALLENGE, foundAuth->authMethod(), challenge};
                _authSession.reset(foundAuth->createSession());
                _authSession->challenge = challenge;
                _authSession->authenticator = foundAuth;
                _authSession->user = foundAuth->user(authId);
                _user = foundAuth->user(authId);
                sendWampMessage(authArr);
                return;
            }

        }
    }
    else if(code == WampMsgCode::AUTHENTICATE)
    {
        _authSession->inBuffer = arr[1].toByteArray();
        AUTH_RESULT authResult = _authSession->authenticate();
        if(authResult == AUTH_RESULT::ACCEPTED)
        {
            if(!_authSession->user) _authSession->user = _authSession->authenticator->getUser(_authSession.data());
            if(_authSession->user)
            {
                welcome();
            }
            else abort(KEY_ERR_NOT_AUTHENTICATED);
        }
        if(authResult == AUTH_RESULT::REJECTED)
        {
            abort(KEY_ERR_NOT_AUTHENTICATED);
        }
        if(authResult == AUTH_RESULT::CONTINUE)
        {
            QVariantMap obj;
            obj["challenge"] = _authSession->outBuffer;
            QVariantList authArr{WampMsgCode::CHALLENGE, _authSession->authenticator->authMethod(), obj};
            sendWampMessage(authArr);
        }
    }
    else if(code == WampMsgCode::REGISTER)
    {
        qulonglong requestId = arr[1].toULongLong();
        QString uri = arr[3].toString();
        bool authorized = authorize(uri, WampMsgCode::REGISTER, requestId);
        if(!authorized) return;
        WampRouterRegistrationPointer registration(new WampRouterRegistration(Random::generate(), uri, q));
        _realm->d_ptr->insertRegistration(uri, registration);
        _registrations.append(registration);
        QVariantList onCreateArgs{_sessionId};
        QVariantMap details;
        details["id"] = registration->registrationId();
        details["created"] = registration->created().toString("yyyy-mm-ddThh:mm:zzzZ");
        details["uri"] = uri;
        onCreateArgs.append(details);
        _realm->publish(KEY_REGISTRATION_ON_CREATE, onCreateArgs);
        QVariantList resArr{WampMsgCode::REGISTERED, requestId, registration->registrationId()};
        sendWampMessage(resArr);
    }
    else if(code == WampMsgCode::UNREGISTER)
    {
        qulonglong requestId = arr[1].toULongLong();
        qulonglong registrationId = arr[2].toULongLong();
        if(_realm->d_ptr->_idRegistartion.contains(registrationId))
        {
            WampRouterRegistrationPointer reg = _realm->d_ptr->getRegistration(registrationId);
            bool authorized = authorize(reg->uri(), WampMsgCode::REGISTER, requestId);
            if(!authorized) return;
            _realm->d_ptr->removeRegistration(reg);
            QVariantList resArr{WampMsgCode::UNREGISTERED, requestId};
            sendWampMessage(resArr);
        }
        else
        {
            error(WampMsgCode::UNREGISTER, KEY_ERR_NO_SUCH_REGISTRATION, requestId);
        }
    }
    else if(code == WampMsgCode::CALL)
    {
        qulonglong requestId = arr[1].toULongLong();
        QString uri = arr[3].toString();
        QVariantList params;
        if(arr.count()>4) params = arr[4].toList();
        bool authorized = authorize(uri, WampMsgCode::CALL, requestId);
        if(!authorized) return;
        if(_realm->containsRegistration(uri))
        {
            WampRouterRegistrationPointer reg = _realm->d_ptr->getRegistration(uri);
            _realm->d_ptr->insertPendingInvocation(requestId, q);
            reg->handleRemote(requestId, q, params);
        }
        else if(_realm->d_ptr->containsInternalRegistration(uri))
        {
            RegistrationPointer impl = _realm->d_ptr->getInternalRegistration(uri);
            WampResult res = impl->execute(params);
            if(res.isError()) error(WampMsgCode::CALL, res.errorUri(), requestId);
            else result(requestId, res.resultData());
        }
        else
        {
            error(WampMsgCode::CALL, KEY_ERR_NO_SUCH_PROCEDURE, requestId, {{"procedureUri", uri}});
        }
    }
    else if(code == WampMsgCode::YIELD)
    {
        qulonglong requstId = arr[1].toULongLong();
        WampRouterSession* caller = _realm->d_ptr->takePendingInvocation(requstId);
        QVariantList resultArr;
        if(arr.count()>3) resultArr = arr[3].toList();
        caller->result(requstId, resultArr);
    }
    else if(code == WampMsgCode::SUBSCRIBE)
    {
        qulonglong requestId = arr[1].toULongLong();
        QString topic = arr[3].toString();
        bool authorized = authorize(topic, WampMsgCode::SUBSCRIBE, requestId);
        if(!authorized) return;

        qulonglong subscriptionId = Random::generate();
        WampRouterSubscriptionPointer subscription(new WampRouterSubscription(topic, subscriptionId, q));
        if(!_realm->d_ptr->containsSubscription(topic))
        {
            QVariantList onCreateArgs{_sessionId};
            QVariantMap details;
            details["id"] = subscriptionId;
            details["created"] = subscription->created().toString("yyyy-mm-ddThh:mm:zzzZ");
            details["uri"] = topic;
            onCreateArgs.append(details);
            _realm->publish(KEY_SUBSCRIPTION_ON_CREATE, onCreateArgs);
        }
        _realm->publish(KEY_SUBSCRIPTION_ON_SUBSCRIBE, {_sessionId, subscriptionId, topic});

        _realm->d_ptr->insertSubscription(subscriptionId, topic, subscription);
        _subscriptions.append(subscription);

        QVariantList resArr{WampMsgCode::SUBSCRIBED, requestId, subscriptionId};
        sendWampMessage(resArr);
    }
    else if(code == WampMsgCode::UNSUBSCRIBE)
    {
        qulonglong requestId = arr[1].toULongLong();
        qulonglong subscriptionId = arr[2].toULongLong();
        WampRouterSubscriptionPointer sub = _realm->d_ptr->takeSubscription(subscriptionId);
        _subscriptions.removeAll(sub);
        QVariantList resArr{WampMsgCode::UNSUBSCRIBED, requestId};
        sendWampMessage(resArr);
    }
    else if(code == WampMsgCode::PUBLISH)
    {
        qulonglong requestId = arr[1].toULongLong();
        QString topic = arr[3].toString();
        QVariantList args;
        if(arr.count() > 4) args = arr[4].toList();
        bool authorized = authorize(topic, WampMsgCode::PUBLISH, requestId);
        if(!authorized) return;

        qulonglong publicationId = _realm->publish(topic, args);

        QVariantList resArr{WampMsgCode::PUBLISHED, requestId, publicationId};
        sendWampMessage(resArr);
    }
}

void WampRouterSession::run()
{
    exec();
}
WampRouterSession::~WampRouterSession()
{
    Q_D(WampRouterSession);
    QMetaObject::invokeMethod(d, "deleteLater", Qt::BlockingQueuedConnection);
    exit();
    wait();
}
qulonglong WampRouterSession::sessionId() const
{
    Q_D(const WampRouterSession);
    return d->_sessionId;
}
void WampRouterSession::sendWampMessage(const QVariantList &arr)
{
    Q_D(WampRouterSession);
    QMetaObject::invokeMethod(d, "sendWampMessage", Qt::QueuedConnection, Q_ARG(QVariantList, arr));
}

void WampRouterSessionPrivate::error(WampMsgCode code, QString uri, qulonglong requestId, QVariantMap details)
{
    QVariantList errArr{WampMsgCode::ERROR, (int)code, requestId, details, uri};
    sendWampMessage(errArr);
}
void WampRouterSessionPrivate::welcome()
{
    QVariantMap roles{{"broker", QVariantMap()}, {"dealer", QVariantMap()}};
    QVariantMap details{{"roles", roles}};
    QVariantList resArr{WampMsgCode::WELCOME, _sessionId, details};
    sendWampMessage(resArr);
}
void WampRouterSessionPrivate::abort(QString uri, QString message)
{
    QVariantMap details;
    if(!message.isEmpty()) details["message"] = message;
    QVariantList abortArr{WampMsgCode::ABORT, details, uri};
    sendWampMessage(abortArr);
}

void WampRouterSessionPrivate::result(qulonglong requestId, QVariant result)
{
    QVariantList responseArr{WampMsgCode::RESULT, requestId, QVariantMap()};

    if(result.isValid())
    {
        QVariantList resultArr;
        if((QMetaType::Type)result.type() == QMetaType::QVariantList)
        {
            resultArr = result.toList();
        }
        else
        {
            resultArr.append(result);
        }
        if(!resultArr.isEmpty()) responseArr.append(QVariant(resultArr));
    }

    //if(arr.count()>4) resArr.append(arr[4]);
    sendWampMessage(responseArr);
}
void WampRouterSession::result(qulonglong requestId, QVariant result)
{
    Q_D(WampRouterSession);
    QMetaObject::invokeMethod(d, "result", Qt::QueuedConnection, Q_ARG(qulonglong, requestId), Q_ARG(QVariant, result));
}

Realm* WampRouterSession::realm() const
{
    Q_D(const WampRouterSession);
    return d->_realm;
}
User* WampRouterSession::user() const
{
    Q_D(const WampRouterSession);
    return d->_user;
}
void WampRouterSession::setUser(User* value)
{
    Q_D(WampRouterSession);
    d->_user = value;
}
bool WampRouterSessionPrivate::authorize(QString uri, WampMsgCode action, qulonglong requestId)
{
    if(_user)
    {
        bool authorized = _user->authorize(uri, action);
        if(!authorized)
        {
            error(action, KEY_ERR_NOT_AUTHORIZED, requestId);
        }
        return authorized;
    }
    return false;
}
QString WampRouterSession::peerAddress() const
{
    Q_D(const WampRouterSession);
    return d->_socket->peerAddress().toString();
}
void WampRouterSessionPrivate::closed()
{
    Q_Q(WampRouterSession);
    for (auto reg: _registrations) {
        _realm->d_ptr->removeRegistration(reg);
    }
    for (auto sub: _subscriptions) {
        _realm->d_ptr->takeSubscription(sub->subscriptionId());
    }
    Q_EMIT q->closed();
}

}
