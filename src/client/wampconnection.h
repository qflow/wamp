#ifndef WAMPCONNECTION_H
#define WAMPCONNECTION_H

#include "wamp_global.h"
#include "wampbase.h"
#include "subscription_p.h"
#include "wamperror.h"
#include "future.h"
#include "user.h"
#include <QUrl>
#include <QObject>
#include <QQmlListReference>


namespace QFlow{

typedef std::function<void(QVariant)> ResultCallback;

class WampAttached;
class WampConnectionPrivate;
class WAMP_EXPORT WampConnection : public WampBase
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString realm READ realm WRITE setRealm NOTIFY realmChanged)
    Q_PROPERTY(User* user READ user WRITE setUser NOTIFY userChanged)

    friend class WampRouterPrivate;
public:
    WampConnection(QObject* parent = NULL);
    ~WampConnection();
    QUrl url() const;
    void setUrl(QUrl value);
    QString realm() const;
    void setRealm(QString realm);
    User* user() const;
    void setUser(User* value);
    template<typename ... Args>
    void subscribe(QString uri, std::function<void(Args...)> f)
    {
        Functor<void, Args...>* functor = new Functor<void,Args...>(f);
        SubscriptionPointer sub(new FunctorSubscription(uri, functor));
        addSubscription(sub);
    }
    void unregister(qulonglong registrationId);
    void unsubscribe(qulonglong subscriptionId);
    Future call2(QString uri, const QVariantList& args, ResultCallback callback = nullptr);
public Q_SLOTS:
    void connect();
    void subscribe(QString uri, QJSValue callback);
    void subscribe(QString uri, QObject* obj, QString method);
    void unregister(QString uri);
    void unsubscribe(QString uri);
    Future lookupRegistration(QString uri);
    Future listRegistrations();
    Future getSubscription(qulonglong subscriptionId);
    Future subscribersCount(QString topicUri, ResultCallback callback = nullptr);
    void publish(QString uri, const QVariantList& args);
    Future call(QString uri, const QVariantList& args, const QJSValue& callback = QJSValue());
    Future call(QString uri, const QVariantList& args, QObject* callbackObj, QString callbackMethod);
    void define(QString uri, QString definition);
    Future describe(QString uri);
Q_SIGNALS:
    void urlChanged();
    void realmChanged();
    void connected();
    void disconnected();
    void error(const WampError& error);
    void userChanged();
    void textMessageReceived(const QString &message);
    void subscriptionCreated(const QString &topicUri);
    void subscriptionDeleted(const QString &topicUrl);
private:
    void addRegistration(RegistrationPointer reg);
    void addSubscription(SubscriptionPointer sub);
    void addSignalObserver(QString uri, SignalObserver* observer);
    const QScopedPointer<WampConnectionPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WampConnection)
};
typedef QSharedPointer<WampConnection> WampConnectionPointer;
}
#endif // WAMPCONNECTION_H
