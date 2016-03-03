#include "wamprouter.h"
#include "wamprouter_p.h"
#include "wamprouterworker.h"
#include "wampconnection.h"
#include "wampconnection_p.h"
#include "wampworker.h"
#include <QJsonDocument>

namespace QFlow{

WampRouterPrivate::WampRouterPrivate(WampRouter* parent) : QObject(), _port(8080), q_ptr(parent)
{
    _worker = new WampRouterWorker();
    _workerThread = new QThread();
    _worker->_router = this;
    connect(_workerThread, &QThread::finished, _workerThread, &QObject::deleteLater);
    _workerThread->start();
    _worker->moveToThread(_workerThread);
    connect(_workerThread, &QThread::finished, _worker, &QObject::deleteLater);
}
WampRouterPrivate::~WampRouterPrivate()
{
}
void WampRouterPrivate::messageReceived(QVariantList message)
{
    Q_Q(WampRouter);
    Q_EMIT q->messageReceived((WampRouterSession*) sender(), message);
}
void WampRouterPrivate::messageSent(QVariantList message)
{
    Q_Q(WampRouter);
    Q_EMIT q->messageSent((WampRouterSession*) sender(), message);
}
WampRouter::WampRouter(QObject *parent) : QObject(parent), d_ptr(new WampRouterPrivate(this))
{

}
WampRouter::~WampRouter()
{
}
QString WampRouter::host() const
{
    Q_D(const WampRouter);
    return d->_host;
}
void WampRouter::setHost(QString value)
{
    Q_D(WampRouter);
    d->_host = value;
    Q_EMIT hostChanged();
}
int WampRouter::port() const
{
    Q_D(const WampRouter);
    return d->_port;
}
void WampRouter::setPort(int value)
{
    Q_D(WampRouter);
    d->_port = value;
    Q_EMIT portChanged();
}
ErrorInfo WampRouter::init()
{
    Q_D(WampRouter);

    for(Realm* realm: d->_worker->_realms) {
        realm->setParent(NULL);
        realm->moveToThread(d->_workerThread);
    }
    QMetaObject::invokeMethod(d->_worker, "startServer", Qt::QueuedConnection);
    return ErrorInfo();
}
ErrorInfo WampRouter::deinit()
{
    return ErrorInfo();
}
QQmlListProperty<QFlow::Realm> WampRouter::realms()
{
    Q_D(WampRouter);
    return QQmlListProperty<QFlow::Realm>(d->_worker, d->_worker->_realms);
}
void WampRouterSubscription::event(qulonglong publicationId, const QVariantList& args)
{
    QVariantList resArr{(int)WampMsgCode::EVENT, _subscriptionId, publicationId, QVariantMap()};
    if(!args.isEmpty()) resArr.append(QVariant(args));
    _subscriber->sendWampMessage(resArr);
}
void WampRouterRegistration::handleRemote(qulonglong requestId, WampRouterSession* /*caller*/, QVariantList params)
{
    QVariantList invArr{(int)WampMsgCode::INVOCATION, requestId, registrationId(), QVariantMap()};
    if(!params.isEmpty()) invArr.append(QVariant(params));
    //if(arr.count()>5) invArr.append(arr[5]);
    _session->sendWampMessage(invArr);
}
}
