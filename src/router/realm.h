#ifndef REALM_H
#define REALM_H

#include "wamp_global.h"
#include "wampbase.h"
#include <QObject>
#include <QQmlListProperty>
#include <QJSValue>
#include <QUrl>

namespace QFlow{

class Authenticator;
class Role;
class RealmPrivate;
class WAMP_EXPORT Realm : public WampBase
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QQmlListProperty<QFlow::Role> roles READ roles)
    Q_PROPERTY(QQmlListProperty<QFlow::Authenticator> authenticators READ authenticators)
    friend class WampRouterWorker;
    friend class WampRouterSessionPrivate;
public:
    explicit Realm(QObject* parent = NULL);
    QString name() const;
    void setName(QString value);
    QQmlListProperty<QFlow::Role> roles();
    QQmlListProperty<QFlow::Authenticator> authenticators();
    Authenticator* findAuthenticatorByAuthMethod(QString authMethod) const;
    ~Realm();
public Q_SLOTS:
    bool containsRegistration(QString uri);
    QStringList registeredUris();
    QStringList registeredInternalUris();
    QVariantList registrationIds();
    qulonglong publish(QString topic, const QVariantList& args);
    int subscribersCount(QString topicUri);
protected:
    void addRegistration(RegistrationPointer reg);
    void addSignalObserver(QString uri, SignalObserver* observer);
private:
    const QScopedPointer<RealmPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Realm)
};
}

#endif // REALM_H
