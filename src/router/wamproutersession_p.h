#ifndef WAMPROUTERSESSION_P_H
#define WAMPROUTERSESSION_P_H

#include "authenticator.h"
#include <QObject>
#include <QPointer>

namespace QFlow{

class WampRouterWorker;
class Realm;
class WebSocketConnection;
class User;
class WampMessageSerializer;
class WampRouterRegistration;
class WampRouterSubscription;
typedef QSharedPointer<WampRouterRegistration> WampRouterRegistrationPointer;
typedef QSharedPointer<WampRouterSubscription> WampRouterSubscriptionPointer;

class WampRouterSessionPrivate : public QObject
{
    Q_OBJECT
public:
    WampRouterSessionPrivate(WampRouterSession* parent);
    ~WampRouterSessionPrivate();
    qulonglong _sessionId;
    QPointer<Realm> _realm;
    QPointer<WebSocketConnection> _socket;
    QScopedPointer<WampMessageSerializer> _serializer;
    QList<WampRouterRegistrationPointer> _registrations;
    QList<WampRouterSubscriptionPointer> _subscriptions;
    WampRouterWorker* _router;
    QScopedPointer<AuthSession> _authSession;
    QString _authId;
public Q_SLOTS:
    void sendWampMessage(const QVariantList& arr);
    void onMessageReceived(const QByteArray &message);
    void abort(QString uri, QString message = QString());
    void welcome();
    bool authorize(QString uri, WampMsgCode action, qulonglong requestId);
    void error(WampMsgCode code, QString uri, qulonglong requestId, QVariantMap details = QVariantMap());
    void result(qulonglong requestId, QVariant result = QVariant());
    void closed();
private:
    WampRouterSession* q_ptr;
    Q_DECLARE_PUBLIC(WampRouterSession)

};
}
#endif // WAMPROUTERSESSION_P_H
