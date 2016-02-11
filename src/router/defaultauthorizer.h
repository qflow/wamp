#ifndef DEFAULTAUTHORIZER_H
#define DEFAULTAUTHORIZER_H

#include "wamp_global.h"
#include "authorizer.h"
#include <QObject>

namespace QFlow{

class DefaultAuthorizerPrivate;
class WAMP_EXPORT DefaultAuthorizer : public Authorizer
{
    Q_OBJECT
    Q_PROPERTY(bool allowCall READ allowCall WRITE setAllowCall NOTIFY allowCallChanged)
    Q_PROPERTY(bool allowRegister READ allowRegister WRITE setAllowRegister NOTIFY allowRegisterChanged)
    Q_PROPERTY(bool allowSubscribe READ allowSubscribe WRITE setAllowSubscribe NOTIFY allowSubscribeChanged)
    Q_PROPERTY(bool allowPublish READ allowPublish WRITE setAllowPublish NOTIFY allowPublishChanged)
    Q_PROPERTY(QString uriPattern READ uriPattern WRITE setUriPattern NOTIFY uriPatternChanged)
public:
    explicit DefaultAuthorizer(QObject *parent = 0);
    ~DefaultAuthorizer();
    bool allowCall() const;
    void setAllowCall(bool value);
    bool allowRegister() const;
    void setAllowRegister(bool value);
    bool allowSubscribe() const;
    void setAllowSubscribe(bool value);
    bool allowPublish() const;
    void setAllowPublish(bool value);
    QString uriPattern() const;
    void setUriPattern(QString value);
    bool authorize(QString authId, QString uri, WampMsgCode action);
Q_SIGNALS:
    void allowCallChanged();
    void allowRegisterChanged();
    void allowSubscribeChanged();
    void allowPublishChanged();
    void uriPatternChanged();
public Q_SLOTS:

private:
    const QScopedPointer<DefaultAuthorizerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DefaultAuthorizer)
};
}
#endif // DEFAULTAUTHORIZER_H
