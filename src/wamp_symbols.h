#ifndef WAMP_SYMBOLS_H
#define WAMP_SYMBOLS_H

#include <QString>

namespace QFlow{
const QString KEY_SUBSCRIPTION_ON_DELETE = QStringLiteral("wamp.subscription.on_delete");
const QString KEY_SUBSCRIPTION_ON_CREATE = QStringLiteral("wamp.subscription.on_create");
const QString KEY_SUBSCRIPTION_ON_SUBSCRIBE = QStringLiteral("wamp.subscription.on_subscribe");
const QString KEY_SUBSCRIPTION_ON_UNSUBSCRIBE = QStringLiteral("wamp.subscription.on_unsubscribe");
const QString KEY_REGISTRATION_ON_DELETE = QStringLiteral("wamp.registration.on_delete");
const QString KEY_REGISTRATION_ON_CREATE = QStringLiteral("wamp.registration.on_create");
const QString KEY_GET_SUBSCRIPTION = QStringLiteral("wamp.subscription.get");
const QString KEY_COUNT_SUBSCRIBERS = QStringLiteral("wamp.subscription.count_subscribers");
const QString KEY_DEFINE_SCHEMA = QStringLiteral("wamp.schema.define");
const QString KEY_DESCRIBE_SCHEMA = QStringLiteral("wamp.schema.describe");
const QString KEY_LOOKUP_REGISTRATION = QStringLiteral("wamp.registration.lookup");
const QString KEY_LIST_REGISTRATION_URIS = QStringLiteral("wamp.registration.list_uris");
const QString KEY_ERR_NOT_AUTHORIZED = QStringLiteral("wamp.error.not_authorized");
const QString KEY_ERR_NOT_AUTHENTICATED = QStringLiteral("wamp.error.not_authenticated");
const QString KEY_ERR_NO_SUCH_PROCEDURE = QStringLiteral("wamp.error.no_such_procedure");
const QString KEY_ERR_NO_SUCH_REGISTRATION = QStringLiteral("wamp.error.no_such_registration");
const QString KEY_ERR_NO_SUCH_REALM = QStringLiteral("wamp.error.no_such_realm");
const QString KEY_WAMP_JSON_SUB = QStringLiteral("wamp.2.json");
const QString KEY_WAMP_MSGPACK_SUB = QStringLiteral("wamp.2.msgpack");

enum WampMsgCode : int {
    HELLO = 1,
    WELCOME = 2,
    ABORT = 3,
    CHALLENGE = 4,
    AUTHENTICATE = 5,
    GOODBYE = 6,
    HEARTBEAT = 7,
    ERROR = 8,
    PUBLISH = 16,
    PUBLISHED = 17,
    SUBSCRIBE = 32,
    SUBSCRIBED = 33,
    UNSUBSCRIBE = 34,
    UNSUBSCRIBED = 35,
    EVENT = 36,
    CALL = 48,
    CANCEL = 49,
    RESULT = 50,
    REGISTER = 64,
    REGISTERED = 65,
    UNREGISTER = 66,
    UNREGISTERED = 67,
    INVOCATION = 68,
    INTERRUPT = 69,
    YIELD = 70
};
}

#endif // WAMP_SYMBOLS_H
