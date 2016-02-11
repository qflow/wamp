#ifndef ROLE_H
#define ROLE_H

#include "wamp_global.h"
#include "authorizer.h"
#include <QObject>

namespace QFlow{

class RolePrivate;
class WAMP_EXPORT Role : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Authorizer* authorizer READ authorizer WRITE setAuthorizer)
public:
    explicit Role(QObject *parent = 0);
    ~Role();
    QString name() const;
    void setName(QString value);
    Authorizer* authorizer() const;
    void setAuthorizer(Authorizer* value);
Q_SIGNALS:

public Q_SLOTS:
private:
    const QScopedPointer<RolePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Role)
};
}
#endif // ROLE_H
