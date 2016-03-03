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
#include <QHostAddress>

namespace QFlow{

WampRouterWorker::WampRouterWorker(QObject *parent) : QObject(parent)
{
}

WampRouterWorker::~WampRouterWorker()
{

}
void WampRouterWorker::startServer()
{
    for(Realm* realm: _realms) {
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
    qDebug() << QString("Web Socket connection opened. Requested subprotocols: %1 Peer Address: %2").arg(subprotocols.join(",")).arg(con->peerAddress().toString());
    QString selectedSub;
    for(QString sub: subprotocols)
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
    QObject::connect(newSession.data(), SIGNAL(messageReceived(QVariantList)), _router, SLOT(messageReceived(QVariantList)));
    QObject::connect(newSession.data(), SIGNAL(messageSent(QVariantList)), _router, SLOT(messageSent(QVariantList)));
    _sessions.insert(newSession->sessionId(), newSession);
    Q_EMIT _router->q_ptr->newSession(newSession.data());
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
