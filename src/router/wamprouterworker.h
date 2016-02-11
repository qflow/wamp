#ifndef WAMPROUTERWORKER_H
#define WAMPROUTERWORKER_H

#include "websocketserver.h"
#include <QPointer>
#include <QJsonObject>

namespace QFlow{

class AuthSession;
class Realm;
class WampRouterSession;
typedef QSharedPointer<WampRouterSession> WampRouterSessionPointer;
class WampRouterPrivate;
class WampMessageSerializer;
class WebSocketConnection;
typedef QSharedPointer<WampMessageSerializer> WampMessageSerializerPointer;

class WampRouterWorker : public QObject
{
    Q_OBJECT
public:
    explicit WampRouterWorker(QObject *parent = 0);
    ~WampRouterWorker();
    WampRouterPrivate* _router;
    QScopedPointer<WebSocketServer> _server;
    QHash<qulonglong, WampRouterSessionPointer> _sessions;
    QList<QFlow::Realm*> _realms;
Q_SIGNALS:

public Q_SLOTS:
    void startServer();
    void onNewConnection(WebSocketConnection* con);
    void sessionClosed();
};
}
#endif // WAMPROUTERWORKER_H
