#ifndef REALM_P_H
#define REALM_P_H

#include <QMutex>
#include <QHash>
#include <QSharedPointer>
#include <QUrl>

class QWebSocket;

namespace QFlow{

class WampRouterSession;
typedef QSharedPointer<WampRouterSession> WampRouterSessionPointer;
class Authenticator;
class Role;
class WampRouterRegistration;
typedef QSharedPointer<WampRouterRegistration> WampRouterRegistrationPointer;
class WampRouterSubscription;
typedef QSharedPointer<WampRouterSubscription> WampRouterSubscriptionPointer;
class Registration;
typedef QSharedPointer<Registration> RegistrationPointer;

class RealmPrivate
{
public:
    QMutex _mutex;
    QString _name;
    QHash<QString, WampRouterRegistrationPointer> _uriRegistartion;
    QHash<qulonglong, WampRouterRegistrationPointer> _idRegistartion;
    QHash<qulonglong, WampRouterSession*> _pendingInvocations;
    void insertPendingInvocation(qulonglong requestId, WampRouterSession* session);
    QList<Role*> _roles;
    QList<Authenticator*> _authenticators;
    QHash<QString, RegistrationPointer> _internalRegistrations;
    void insertRegistration(QString uri, WampRouterRegistrationPointer registration);
    WampRouterRegistrationPointer getRegistration(qulonglong registrationId);
    WampRouterRegistrationPointer getRegistration(QString uri);
    void removeRegistration(WampRouterRegistrationPointer reg);
    bool containsInternalRegistration(QString uri);
    RegistrationPointer getInternalRegistration(QString uri);
    WampRouterSession* takePendingInvocation(qulonglong requestId);

    QHash<qulonglong, WampRouterSubscriptionPointer> _subscriptions;
    WampRouterSubscriptionPointer takeSubscription(qulonglong subscriptionId);
    void insertSubscription(qulonglong subscriptionId, QString topic, WampRouterSubscriptionPointer subscription);
    QMultiHash<QString, WampRouterSubscriptionPointer> _uriSubscriptions;
    bool containsSubscription(QString topic);
    bool containsSubscription(qulonglong subscriptionId);
    qulonglong publish(QString topic, const QVariantList& args);

    RealmPrivate();
    ~RealmPrivate();
};
}

#endif // REALM_P_H

