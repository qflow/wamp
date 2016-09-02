#ifndef SID_H
#define SID_H

#include "user.h"
#include "initiable.h"
#include <QObject>
#include <QVariant>

namespace QFlow{

class SIDUserPrivate;
class WAMP_EXPORT SIDUser : public User, public QmlInitiable
{
    Q_OBJECT
public:
    SIDUser(QObject* parent = NULL);
    ~SIDUser();
    QByteArray response(QByteArray challenge);
    QString authMethod() const;
    bool checkTokenMembership(QVariant handle);
    ErrorInfo init();
protected:
    SIDUser(SIDUserPrivate &dd, QObject *parent);
};
}
#endif // SID_H
