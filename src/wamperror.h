#ifndef WAMPERROR_H
#define WAMPERROR_H

#include "wamp_global.h"
#include <QExplicitlySharedDataPointer>
#include <QVariantMap>
#include <QUrl>
#include <QDebug>

namespace QFlow{

class WampErrorPrivate;
class WAMP_EXPORT WampError
{
    Q_GADGET
    Q_PROPERTY(int requestType READ requestType)
    Q_PROPERTY(qulonglong requestId READ requestId)
    Q_PROPERTY(QVariantMap details READ details)
    Q_PROPERTY(QUrl uri READ uri)
    Q_PROPERTY(QVariantList args READ args)
public:
    WampError(int requestType, qulonglong requestId, QVariantMap details, QUrl uri, QVariantList args);
    WampError();
    WampError(const WampError& other);
    WampError &operator=(const WampError &other);
    ~WampError();
    int requestType() const;
    qulonglong requestId() const;
    QVariantMap details() const;
    QUrl uri() const;
    QVariantList args() const;
    Q_INVOKABLE QString toString() const;
private:
    QExplicitlySharedDataPointer<WampErrorPrivate> d;
};
}
#endif // WAMPERROR_H
