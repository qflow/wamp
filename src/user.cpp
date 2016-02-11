#include "user.h"
#include "user_p.h"

namespace QFlow{

UserPrivate::UserPrivate()
{
}
UserPrivate::~UserPrivate()
{

}

User::User(QObject* parent) : QObject(parent), d_ptr(new UserPrivate())
{

}
User::User(UserPrivate &dd, QObject *parent) : QObject(parent), d_ptr(&dd)
{

}

User::~User()
{

}

QString User::name() const
{
    Q_D(const User);
    return d->_name;
}
void User::setName(QString value)
{
    Q_D(User);
    d->_name = value;
}

Role* User::role() const
{
    Q_D(const User);
    return d->_role;
}
void User::setRole(Role* role)
{
    Q_D(User);
    d->_role = role;
}

bool User::authorize(QString uri, WampMsgCode action)
{
    Q_D(User);
    return d->_role->authorizer()->authorize(d->_name, uri, action);
}
}

