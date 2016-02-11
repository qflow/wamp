#ifndef GSSAPIAUTHENTICATOR_H
#define GSSAPIAUTHENTICATOR_H

#include "authenticator.h"
#include <QObject>


namespace QFlow{
class GSSAPIAuthenticatorPrivate;
class GSSAPIAuthenticator : public Authenticator
{
    Q_OBJECT
    Q_PROPERTY(QString authMethod READ authMethod WRITE setAuthMethod)
public:
    explicit GSSAPIAuthenticator(QObject *parent = 0);
    ~GSSAPIAuthenticator();
    QString authMethod() const;
    void setAuthMethod(const QString& value);
    QVariantMap generateChallenge(qulonglong sessionId, QString authId);
    AuthSession* createSession() const;
    User* getUser(AuthSession* session);
Q_SIGNALS:

public Q_SLOTS:
private:
    const QScopedPointer<GSSAPIAuthenticatorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(GSSAPIAuthenticator)
};
}
#endif // GSSAPIAUTHENTICATOR_H
