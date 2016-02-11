#ifndef WAMPCRAAUTHENTICATOR_H
#define WAMPCRAAUTHENTICATOR_H

#include "wamp_global.h"
#include "authenticator.h"
#include "wampcrauser.h"
#include <QObject>
#include <QQmlListProperty>

namespace QFlow{

class WampCraAuthenticatorPrivate;
class WAMP_EXPORT WampCraAuthenticator : public Authenticator
{
    Q_OBJECT
public:
    explicit WampCraAuthenticator(QObject *parent = 0);
    ~WampCraAuthenticator();
    QVariantMap generateChallenge(qulonglong sessionId, QString authId);
    QString authMethod() const;
    AuthSession* createSession() const override;
    User* getUser(AuthSession* session);
Q_SIGNALS:

public Q_SLOTS:
private:
    const QScopedPointer<WampCraAuthenticatorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WampCraAuthenticator)
};
}
#endif // WAMPCRAAUTHENTICATOR_H
