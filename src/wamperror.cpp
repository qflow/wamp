#include "wamperror.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

namespace QFlow{

class WampErrorPrivate : public QSharedData
{
public:
    WampErrorPrivate() : QSharedData()
    {

    }
    WampErrorPrivate(const WampErrorPrivate& other) : QSharedData(other),
        _requestType(other._requestType), _requestId(other._requestId), _details(other._details), _uri(other._uri),
        _args(other._args)
    {

    }
    ~WampErrorPrivate()
    {

    }
    int _requestType;
    qulonglong _requestId;
    QVariantMap _details;
    QUrl _uri;
    QVariantList _args;
};

WampError::WampError(int requestType, qulonglong requestId, QVariantMap details, QUrl uri, QVariantList args) : d(new WampErrorPrivate())
{
    d->_requestType = requestType;
    d->_requestId = requestId;
    d->_details = details;
    d->_uri = uri;
    d->_args = args;
}
WampError::WampError() : d(new WampErrorPrivate())
{

}
WampError::~WampError()
{

}

WampError::WampError(const WampError &other) : d(other.d)
{

}
WampError & WampError::operator=(const WampError &other)
{
    if (this == &other)
        return *this;
    d = other.d;
    return *this;
}
int WampError::requestType() const
{
    return d->_requestType;
}
qulonglong WampError::requestId() const
{
    return d->_requestId;
}
QVariantMap WampError::details() const
{
    return d->_details;
}
QUrl WampError::uri() const
{
    return d->_uri;
}
QVariantList WampError::args() const
{
    return d->_args;
}
QString WampError::toString() const
{
    QJsonObject detailsJson = QJsonObject::fromVariantMap(d->_details);
    QJsonDocument detailsDoc(detailsJson);

    QJsonArray argsJson = QJsonArray::fromVariantList(d->_args);
    QJsonDocument argsDoc(argsJson);

    QString result = QString("RequestType: %1\nRequestId: %2\nDetails: %3ErrorUri: %4\nArgs: %5").arg(d->_requestType).arg(d->_requestId)
            .arg(QString(detailsDoc.toJson())).arg(d->_uri.url()).arg(QString(argsDoc.toJson()));
    return result;
}
}
