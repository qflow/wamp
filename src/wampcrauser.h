#ifndef WAMPCRAUSER_H
#define WAMPCRAUSER_H

#include "wamp_global.h"
#include "authenticator.h"
#include "role.h"
#include "user.h"
#include <QObject>

namespace QFlow{

class WampCraUserPrivate;
class WAMP_EXPORT WampCraUser : public User
{
    Q_OBJECT
    Q_PROPERTY(QString secret READ secret WRITE setSecret)
public:
    WampCraUser(QObject* parent = NULL);
    WampCraUser(QString name, QString secret, QObject* parent = NULL);
    ~WampCraUser();
    QString secret() const;
    void setSecret(QString value);
    QByteArray response(QByteArray challenge);
    QString authMethod() const;
protected:
    WampCraUser(WampCraUserPrivate &dd, QObject *parent);
};
}
#endif // WAMPCRAUSER_H
