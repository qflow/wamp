#ifndef WAMPCONNECTION_P_H
#define WAMPCONNECTION_P_H

#include "signalobserver.h"
#include "jsonhelper.h"
#include "registration_p.h"
#include "subscription_p.h"
#include "call.h"
#include <QThread>
#include <QPointer>

namespace QFlow{

class WebSocketConnection;
class WampWorker;
class WampInvocation;
typedef QSharedPointer<WampInvocation> WampInvocationPointer;

class Event
{
public:
    SubscriptionPointer subscription;
    QVariantList args;
    qulonglong publicationId;
    Event() : subscription(NULL)
    {

    }
    ~Event()
    {

    }
    Event(const Event& other) : subscription(other.subscription), args(other.args), publicationId(other.publicationId)
    {

    }
};

class User;
class WampConnection;
class WampMessageSerializer;
class WampConnectionPrivate : public QObject
{
    Q_OBJECT
    friend class WampWorker;
public:

    WampConnectionPrivate(WampConnection* parent);
    ~WampConnectionPrivate();
    QUrl _url;
    QString _realm;
    User* _user;
    QThread _workerThread;
    WampWorker* _worker;
    QScopedPointer<WampMessageSerializer> _serializer;
    QHash<qulonglong,RegistrationPointer> _pendingRegistrations;
    QHash<qulonglong,RegistrationPointer> _pendingUnregistrations;
    QHash<qulonglong,RegistrationPointer> _registrations;
    QHash<QString,RegistrationPointer> _uriRegistration;
    QHash<qulonglong,CallPointer> _pendingCalls;
    QHash<QString, SignalObserverPointer> _topicObserver;

    QHash<qulonglong,SubscriptionPointer> _pendingSubscriptions;
    QHash<qulonglong,SubscriptionPointer> _pendingUnsubscriptions;
    QHash<qulonglong,SubscriptionPointer> _subscriptions;
    QHash<QString,SubscriptionPointer> _uriSubscription;

    void addRegistration(RegistrationPointer reg);
    void addSubscription(SubscriptionPointer sub);

    static QByteArray PBKDF2(QString password, QString salt, int iterations);
    static QByteArray IntToOctet(int i);
    void onConnected();
public Q_SLOTS:
    void handleInvocation(WampInvocationPointer invocation);
    void handleEvent(const Event& event);
    void sendWampMessage(const QVariantList& arr);
    void call(QString uri, const QVariantList& args, CallPointer call);
private:
    WampConnection* q_ptr;
    Q_DECLARE_PUBLIC(WampConnection)
};
}
#endif // WAMPCONNECTION_P_H

