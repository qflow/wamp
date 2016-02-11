#ifndef AUTHORIZER
#define AUTHORIZER

#include "wamp_global.h"
#include "wamp_symbols.h"
#include <QObject>
#include <QUrl>

namespace QFlow{

class WAMP_EXPORT Authorizer : public QObject
{
    Q_OBJECT
public:
    Authorizer(QObject* parent = NULL);
    virtual bool authorize(QString authId, QString uri, WampMsgCode action) = 0;
};
}

#endif // AUTHORIZER

