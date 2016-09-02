#include "authenticator.h"
#include "user.h"


namespace QFlow{

AuthSession::AuthSession() : user(NULL)
{

}
AuthSession::AuthSession(const AuthSession &other) : challenge(other.challenge), authenticator(other.authenticator),
    user(other.user), inBuffer(other.inBuffer)
{

}

AuthSession::~AuthSession()
{

}
Authenticator::Authenticator(QObject *parent) : QObject(parent)
{

}
Authenticator::~Authenticator()
{

}

void Authenticator::users_append(QQmlListProperty<QFlow::User> *prop, User *item)
{
    Authenticator* auth = static_cast<Authenticator*>(prop->object);
    auth->_users.insert(item->name(), item);
}

int Authenticator::users_count(QQmlListProperty<QFlow::User> *prop)
{
    Authenticator* auth = static_cast<Authenticator*>(prop->object);
    return auth->_users.count();
}

User *Authenticator::users_at(QQmlListProperty<QFlow::User> *prop, int index)
{
    Authenticator* auth = static_cast<Authenticator*>(prop->object);
    return auth->_users.values().at(index);
}

void Authenticator::users_clear(QQmlListProperty<QFlow::User> *prop)
{
    Authenticator* auth = static_cast<Authenticator*>(prop->object);
    auth->_users.clear();
}
QQmlListProperty<User> Authenticator::users()
{
    return QQmlListProperty<User>(this, NULL, users_append, users_count, users_at, users_clear);
}

bool Authenticator::containsUser(QString userName) const
{
    return _users.contains(userName);
}
User* Authenticator::user(QString userName) const
{
    return _users[userName];
}
QVariantMap Authenticator::generateChallenge(qulonglong /*sessionId*/, QString /*authId*/)
{
    return QVariantMap();
}

QString Authenticator::authMethod() const
{
    return QString();
}
AuthSession* Authenticator::createSession() const
{
    return nullptr;
}
User* Authenticator::getUser(AuthSession* /*session*/)
{
   return nullptr;
}
}
