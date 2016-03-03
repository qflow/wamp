#ifndef WAMPROUTER_P_H
#define WAMPROUTER_P_H

#include "radixtreenode.h"
#include "wamproutersession.h"
#include "registration_p.h"
#include "subscription_p.h"
#include <QSet>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>

namespace QFlow{

class WampRouterWorker;
class WampRouterSession;
class WampRouterRegistration
{
public:
    WampRouterRegistration(qulonglong regId, QString regUri, WampRouterSession* sessionPtr) :
        _registrationId(regId), _uri(regUri), _session(sessionPtr), _created(QDateTime::currentDateTime())
    {
    }
    ~WampRouterRegistration()
    {

    }
    qulonglong registrationId() const
    {
        return _registrationId;
    }
    QString uri() const
    {
        return _uri;
    }
    void handleRemote(qulonglong requestId, WampRouterSession* caller, QVariantList params);
    QDateTime created() const
    {
        return _created;
    }
    WampRouterSession* callee() const
    {
        return _session;
    }
private:
    qulonglong _registrationId;
    QString _uri;
    WampRouterSession* _session;
    QDateTime _created;
};
typedef QSharedPointer<WampRouterRegistration> WampRouterRegistrationPointer;
class WampRouterSubscription
{
public:
    WampRouterSubscription(QString topic, qulonglong subscriptionId, WampRouterSession* subscriber) : _topic(topic), _subscriptionId(subscriptionId),
        _subscriber(subscriber), _created(QDateTime::currentDateTime())
    {
    }
    ~WampRouterSubscription()
    {

    }

    WampRouterSession* subscriber() const
    {
        return _subscriber;
    }
    QString topic() const
    {
        return _topic;
    }
    qulonglong subscriptionId() const
    {
        return _subscriptionId;
    }
    void event(qulonglong publicationId, const QVariantList& args);
    QDateTime created() const
    {
        return _created;
    }

private:
    QString _topic;
    qulonglong _subscriptionId;
    WampRouterSession* _subscriber;
    QDateTime _created;
};
typedef QSharedPointer<WampRouterSubscription> WampRouterSubscriptionPointer;

class Realm;
class WampRouter;
class WampRouterPrivate : public QObject
{
    Q_OBJECT
public:
    QString _host;
    int _port;
    QThread* _workerThread;
    WampRouterWorker* _worker;

    WampRouterPrivate(WampRouter* parent);
    ~WampRouterPrivate();
public Q_SLOTS:
    void messageReceived(QVariantList message);
    void messageSent(QVariantList message);
public:
    WampRouter* q_ptr;
    Q_DECLARE_PUBLIC(WampRouter)
};
}
#endif // WAMPROUTER_P_H

