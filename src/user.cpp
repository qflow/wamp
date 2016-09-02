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
    return d_ptr->_name;
}
void User::setName(QString value)
{
    d_ptr->_name = value;
}

Role* User::role() const
{
    return d_ptr->_role;
}
void User::setRole(Role* role)
{
    d_ptr->_role = role;
}

bool User::authorize(QString uri, WampMsgCode action)
{
    return d_ptr->_role->authorizer()->authorize(d_ptr->_name, uri, action);
}
}

