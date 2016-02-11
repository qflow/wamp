#ifndef WAMPROUTER_H
#define WAMPROUTER_H

#include "wamp_global.h"
#include "initiable.h"
#include "realm.h"
#include <QObject>
#include <QQmlListProperty>

namespace QFlow{

class WampRouterPrivate;
class WAMP_EXPORT WampRouter : public QObject, public QmlInitiable
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QQmlListProperty<QFlow::Realm> realms READ realms)
    Q_CLASSINFO("DefaultProperty", "realms")
public:
    explicit WampRouter(QObject *parent = 0);
    ~WampRouter();
    QString host() const;
    void setHost(QString value);
    int port() const;
    void setPort(int value);
    Q_INVOKABLE ErrorInfo init();
    Q_INVOKABLE ErrorInfo deinit();
    QQmlListProperty<QFlow::Realm> realms();
Q_SIGNALS:
    void hostChanged();
    void portChanged();
public Q_SLOTS:

private:
    const QScopedPointer<WampRouterPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WampRouter)
};
}
#endif // WAMPROUTER_H
