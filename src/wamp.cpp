#include "wamp.h"
#include "qqml.h"
#include "wampconnection.h"
#include "wampconnection_p.h"
#include "wamperror.h"
#include "wampattached.h"
#include "wamprouter.h"
#include "role.h"
#include "authorizer.h"
#include "authenticator.h"
#include "defaultauthorizer.h"
#include "wampcraauthenticator.h"
#include "gssapiauthenticator.h"
#include "wampinvocation.h"
#include "treemodel.h"
#include "gssapiuser.h"
#include "sid.h"

namespace QFlow{

void Wamp::registerTypes(const char *uri)
{
    // @uri QFlow.Wamp
    qmlRegisterType<WampConnection>(uri, 1, 0, "WampConnection");
    qmlRegisterType<WampRouter>(uri, 1, 0, "WampRouter");
    qmlRegisterType<Realm>(uri, 1, 0, "Realm");
    qmlRegisterType<Role>(uri, 1, 0, "Role");
    qmlRegisterUncreatableType<Authorizer>(uri, 1, 0, "Authorizer", "Cannot instatiate Authorizer base type");
    qmlRegisterType<DefaultAuthorizer>(uri, 1, 0, "DefaultAuthorizer");
    qmlRegisterType<WampCraAuthenticator>(uri, 1, 0, "WampCraAuthenticator");
    qmlRegisterType<GSSAPIAuthenticator>(uri, 1, 0, "GSSAPIAuthenticator");
    qmlRegisterUncreatableType<User>(uri, 1, 0, "User", "Cannot instatiate User base type");
    qmlRegisterType<WampCraUser>(uri, 1, 0, "WampCraUser");
    qmlRegisterType<GSSAPIUser>(uri, 1, 0, "GSSAPIUser");
    qmlRegisterType<SIDUser>(uri, 1, 0, "SID");
    qmlRegisterUncreatableType<Authenticator>(uri, 1, 0, "Authenticator", "Cannot instatiate Authenticator base type");
    qmlRegisterType<TreeModel>(uri, 1, 0, "UriTreeModel");

    qRegisterMetaType<WampInvocationPointer>("WampInvocationPointer");
    qRegisterMetaType<Event>("Event");
    qRegisterMetaType<WampError>("WampError");
    qmlRegisterUncreatableType<WampError>(uri, 1, 0, "WampError", "WampError cannot be created");
    qmlRegisterUncreatableType<WampBase>(uri, 1, 0, "WampBase", "Cannot instatiate WampBase base type");
    QMetaType::registerConverter<WampError, QString>(&WampError::toString);
}
}
