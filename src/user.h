#ifndef USER_H
#define USER_H

#include "wamp_global.h"
#include "role.h"
#include <QObject>

namespace QFlow{

class UserPrivate;
class WAMP_EXPORT User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Role* role READ role WRITE setRole)

public:
    User(QObject* parent = NULL);
    virtual ~User();
    virtual QString name() const;
    void setName(QString value);
    Role* role() const;
    void setRole(Role* role);
    bool authorize(QString uri, WampMsgCode action);
    virtual QByteArray response(QByteArray challenge) = 0;
    virtual QString authMethod() const = 0;
protected:
    const QScopedPointer<UserPrivate> d_ptr;
    User(UserPrivate &dd, QObject *parent);
private:
    Q_DECLARE_PRIVATE(User)
};
}
#endif // USER_H
