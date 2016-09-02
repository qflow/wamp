#ifndef CALL_H
#define CALL_H

#include "future.h"
#include <QObject>

namespace QFlow{

class Impl;

class Call : public QObject
{
    Q_OBJECT
public:
    Call(Impl* callback, QObject* parent = NULL);
    ~Call();
    void resultReady(QVariant res);
    Future getFuture();
    Q_SLOT void resultReadyInternal(QVariant res);
    Q_SIGNAL void result(QVariant res);
private:
    QScopedPointer<Impl> _callback;
    std::promise<QVariant> _promise;
};
struct CallDeleter {
    void operator()(Call* c) const {
        c->deleteLater();
    }
};
typedef QSharedPointer<Call> CallPointer;
}
#endif // CALL_H
