#include "role.h"

namespace QFlow{

class RolePrivate
{
public:
    QString _name;
    Authorizer* _authorizer;
    RolePrivate()
    {

    }
    ~RolePrivate()
    {

    }
};

Role::Role(QObject *parent) : QObject(parent), d_ptr(new RolePrivate())
{

}
Role::~Role()
{

}
QString Role::name() const
{
    Q_D(const Role);
    return d->_name;
}
void Role::setName(QString value)
{
    Q_D(Role);
    d->_name = value;
}
Authorizer* Role::authorizer() const
{
    Q_D(const Role);
    return d->_authorizer;
}
void Role::setAuthorizer(Authorizer *value)
{
    Q_D(Role);
    d->_authorizer = value;
}
}
