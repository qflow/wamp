#ifndef WAMPROUTERSESSION_H
#define WAMPROUTERSESSION_H

#include "wamp_symbols.h"
#include <QObject>
#include <QJsonArray>
#include <QVariant>
#include <QThread>

namespace QFlow{

class Realm;
class User;
class WebSocketConnection;

class WampRouterSessionPrivate;
class WampRouterSession : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString  authId READ authId)
    Q_PROPERTY(qulonglong sessionId READ sessionId)
    Q_PROPERTY(QString peerAddress READ peerAddress)
public:
    WampRouterSession(WebSocketConnection* socket, QString subprotocol, QObject* parent);
    qulonglong sessionId() const;
    ~WampRouterSession();
    Realm* realm() const;
    void invoke(qulonglong requestId, QString uri);
    User* user() const;
    void sendWampMessage(const QVariantList& arr);
    void result(qulonglong requestId, QVariant result);
    QString authId() const;
public Q_SLOTS:
    QString peerAddress() const;
Q_SIGNALS:
    void closed();
    void aborted(QString uri, QString message);
    void messageSent(QVariantList message);
    void messageReceived(QVariantList message);
    void subscribed(QString topic);
    void unsubscribed(QString topic);
    void registered(QString uri);
    void unregistered(QString uri);
    void error(WampMsgCode code, QString uri);
    void welcomed();
private:
    void run();
    WampRouterSessionPrivate* d_ptr;
    Q_DECLARE_PRIVATE(WampRouterSession)
};
}

#endif // WAMPROUTERSESSION_H
