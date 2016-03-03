#ifndef USER_P_H
#define USER_P_H

#include <QString>

namespace QFlow{
class Role;
class UserPrivate
{
public:
    UserPrivate();
    virtual ~UserPrivate();
    QString _name;
    Role* _role;
};
}

#endif // USER_P_H
