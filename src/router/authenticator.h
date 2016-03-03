#ifndef AUTHENTICATOR
#define AUTHENTICATOR

#include "wamp_global.h"
#include "symbols.h"
#include "role.h"
#include <QObject>
#include <QUrl>
#include <QJsonObject>
#include <QPointer>
#include <QQmlListProperty>

namespace QFlow{

class Authenticator;

class WampRouterSession;
typedef QSharedPointer<WampRouterSession> WampRouterSessionPointer;
class User;

enum class AUTH_RESULT {ACCEPTED = 1, REJECTED = 0, CONTINUE = 2};
class AuthSession
{
public:
    QVariantMap challenge;
    QPointer<Authenticator> authenticator;
    QPointer<User> user;
    QByteArray inBuffer;
    QByteArray outBuffer;
    AuthSession();
    AuthSession(const AuthSession &other);
    virtual ~AuthSession();
    virtual AUTH_RESULT authenticate() = 0;
};

class WAMP_EXPORT Authenticator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QFlow::User> users READ users)

    static void users_append(QQmlListProperty<QFlow::User> *prop, User *item);
    static int users_count(QQmlListProperty<QFlow::User> *prop);
    static User *users_at(QQmlListProperty<QFlow::User> *prop, int index);
    static void users_clear(QQmlListProperty<QFlow::User> *prop);
public:
    Authenticator(QObject* parent = NULL);
    virtual QVariantMap generateChallenge(qulonglong sessionId, QString authId) = 0;
    virtual QString authMethod() const = 0;
    QQmlListProperty<User> users();
    bool containsUser(QString userName) const;
    User* user(QString userName) const;
    virtual AuthSession* createSession() const = 0;
    virtual User* getUser(AuthSession* session) = 0;
protected:
    QHash<QString, User*> _users;
};
}

#endif // AUTHENTICATOR

