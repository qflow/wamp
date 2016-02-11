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
public:
    WampRouterSession(WebSocketConnection* socket, QString subprotocol, QObject* parent);
    qulonglong sessionId() const;
    ~WampRouterSession();
    Realm* realm() const;
    void invoke(qulonglong requestId, QString uri);
    User* user() const;
    void setUser(User* value);
    void sendWampMessage(const QVariantList& arr);
    void result(qulonglong requestId, QVariant result);
public Q_SLOTS:
    QString peerAddress() const;
Q_SIGNALS:
    void closed();
private:
    void run();
    WampRouterSessionPrivate* d_ptr;
    Q_DECLARE_PRIVATE(WampRouterSession)
};
}

#endif // WAMPROUTERSESSION_H
