#ifndef GSSAPIUSER_H
#define GSSAPIUSER_H

#include "user.h"
#include <QObject>

namespace QFlow{

class GSSAPIUserPrivate;
class GSSAPIUser : public User
{
    Q_OBJECT
    Q_PROPERTY(QString authMethod READ authMethod WRITE setAuthMethod)
public:
    explicit GSSAPIUser(QObject *parent = 0);
    ~GSSAPIUser();
    QByteArray response(QByteArray challenge);
    QString authMethod() const;
    void setAuthMethod(const QString& value);
Q_SIGNALS:

public Q_SLOTS:
private:
    const QScopedPointer<GSSAPIUserPrivate> d_ptr;
    Q_DECLARE_PRIVATE(GSSAPIUser)
};
}
#endif // GSSAPIUSER_H
