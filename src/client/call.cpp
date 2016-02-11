#include "call.h"
#include "registration_p.h"

namespace QFlow{

Call::Call(Impl* callback, QObject* parent) : QObject(parent), _callback(callback)
{
}
Call::~Call()
{

}
void Call::resultReadyInternal(QVariant res)
{
    if(_callback)
    {
        _callback->execute({res});
    }
    _promise.set_value(res);
}
void Call::resultReady(QVariant res)
{
    if(parent())
    {
        QMetaObject::invokeMethod(this, "resultReadyInternal", Qt::BlockingQueuedConnection, Q_ARG(QVariant, res));
    }
    else resultReadyInternal(res);
}
Future Call::getFuture()
{
    std::shared_future<QVariant> stdFuture(_promise.get_future());
    return Future(stdFuture);
}
}
