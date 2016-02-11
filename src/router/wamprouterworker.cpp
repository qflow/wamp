#include "wamprouterworker.h"
#include "wamprouter_p.h"
#include "wamprouter.h"
#include "wampconnection_p.h"
#include "random.h"
#include "realm_p.h"
#include "wamproutersession.h"
#include "authenticator.h"
#include "credentialstore.h"
#include "wampinvocation.h"
#include "wampmessageserializer.h"

namespace QFlow{

WampRouterWorker::WampRouterWorker(QObject *parent) : QObject(parent)
{
}

WampRouterWorker::~WampRouterWorker()
{

}
void WampRouterWorker::startServer()
{
    Q_FOREACH (Realm* realm, _realms) {
        realm->setParent(this);
    }
    _server.reset(new WebSocketServer());
    _server->setHost(_router->_host);
    _server->setPort(_router->_port);
    QObject::connect(_server.data(), SIGNAL(newConnection(WebSocketConnection*)), this, SLOT(onNewConnection(WebSocketConnection*)));
    _server->init();
    qDebug() << QString("WampRouter started on port %1").arg(_router->_port);
}

void WampRouterWorker::onNewConnection(WebSocketConnection *con)
{
    QStringList subprotocols = con->requestedSubprotocols();
    QString selectedSub;
    Q_FOREACH(QString sub, subprotocols)
    {
        if(sub == KEY_WAMP_JSON_SUB) selectedSub = KEY_WAMP_JSON_SUB;
        else if(sub == KEY_WAMP_MSGPACK_SUB)
        {
            selectedSub = KEY_WAMP_MSGPACK_SUB;
            break;
        }
    }
    if(selectedSub.isEmpty()) con->accept(false);
    con->selectSubprotocol(selectedSub);
    con->accept(true);
    WampRouterSessionPointer newSession(new WampRouterSession(con, selectedSub, this));
    QObject::connect(newSession.data(), SIGNAL(closed()), this, SLOT(sessionClosed()));
    _sessions.insert(newSession->sessionId(), newSession);
}
void WampRouterWorker::sessionClosed()
{
    WampRouterSession* session = (WampRouterSession*)sender();
    if(_sessions.contains(session->sessionId()))
    {
        _sessions.remove(session->sessionId());
    }
}
}
