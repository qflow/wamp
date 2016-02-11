#include "defaultauthorizer.h"
#include "symbols.h"
#include <QRegExp>
#include <QDebug>

namespace QFlow{

class DefaultAuthorizerPrivate
{
public:
    bool _allowCall;
    bool _allowRegister;
    bool _allowSubscribe;
    bool _allowPublish;
    QRegExp _regExp;
    DefaultAuthorizerPrivate() : _allowCall(true), _allowRegister(true), _allowSubscribe(true),
        _allowPublish(true)
    {
        _regExp.setPatternSyntax(QRegExp::Wildcard);
    }
    ~DefaultAuthorizerPrivate()
    {

    }
};


DefaultAuthorizer::DefaultAuthorizer(QObject *parent) : Authorizer(parent), d_ptr(new DefaultAuthorizerPrivate())
{

}
DefaultAuthorizer::~DefaultAuthorizer()
{

}
bool DefaultAuthorizer::allowCall() const
{
    Q_D(const DefaultAuthorizer);
    return d->_allowCall;
}
void DefaultAuthorizer::setAllowCall(bool value)
{
    Q_D(DefaultAuthorizer);
    d->_allowCall = value;
    Q_EMIT allowCallChanged();
}
bool DefaultAuthorizer::allowRegister() const
{
    Q_D(const DefaultAuthorizer);
    return d->_allowRegister;
}
void DefaultAuthorizer::setAllowRegister(bool value)
{
    Q_D(DefaultAuthorizer);
    d->_allowRegister = value;
    Q_EMIT allowRegisterChanged();
}
bool DefaultAuthorizer::allowSubscribe() const
{
    Q_D(const DefaultAuthorizer);
    return d->_allowSubscribe;
}
void DefaultAuthorizer::setAllowSubscribe(bool value)
{
    Q_D(DefaultAuthorizer);
    d->_allowSubscribe = value;
    Q_EMIT allowSubscribeChanged();
}
bool DefaultAuthorizer::allowPublish() const
{
    Q_D(const DefaultAuthorizer);
    return d->_allowPublish;
}
void DefaultAuthorizer::setAllowPublish(bool value)
{
    Q_D(DefaultAuthorizer);
    d->_allowPublish = value;
    Q_EMIT allowPublishChanged();
}
QString DefaultAuthorizer::uriPattern() const
{
    Q_D(const DefaultAuthorizer);
    return d->_regExp.pattern();
}
void DefaultAuthorizer::setUriPattern(QString value)
{
    Q_D(DefaultAuthorizer);
    d->_regExp.setPattern(value);
}

bool DefaultAuthorizer::authorize(QString /*authId*/, QString uri, WampMsgCode action)
{
    Q_D(DefaultAuthorizer);
    bool match = true;
    if(!d->_regExp.isEmpty())
    {
        match = d->_regExp.exactMatch(uri);
    }
    if(action == WampMsgCode::CALL)
    {
        if(!d->_allowCall) return false;
        return match;
    }
    else if(action == WampMsgCode::PUBLISH)
    {
        return d->_allowPublish;
    }
    else if(action == WampMsgCode::SUBSCRIBE)
    {
        if(!d->_allowSubscribe) return false;
        return match;
    }
    else if(action == WampMsgCode::REGISTER)
    {
        return d->_allowRegister;
    }
    return false;
}
}
