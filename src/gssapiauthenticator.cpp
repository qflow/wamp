#include "gssapiauthenticator.h"
#include "sid.h"
#include <QByteArray>

#ifdef Q_OS_WIN
#include <Windows.h>
#define SECURITY_WIN32
#include <Security.h>
#endif

namespace QFlow{

class GSSAPIAuthenticatorPrivate
{
public:
    QString _authMethod;
    GSSAPIAuthenticatorPrivate()
    {

    }
    ~GSSAPIAuthenticatorPrivate()
    {

    }
};
class GSSAPISession: public AuthSession
{
public:
    QString _authMethod;
    bool _first;
#ifdef Q_OS_WIN
    CredHandle _serverHandle;
    SecBufferDesc _outSecBufDesc;
    SecBuffer _outSecBuf;
    SecBufferDesc _inSecBufDesc;
    SecBuffer _inSecBuf;
    PSecPkgInfo _info;
    CtxtHandle _context;
    ULONG _attrs;
    TimeStamp _serviceLifetime;
    HANDLE _secToken;
    SecPkgContext_Names  _names;
#endif
    GSSAPISession(QString authMethod) : _authMethod(authMethod), _first(true)
    {
#ifdef Q_OS_WIN
        SECURITY_STATUS status;

        LPWSTR methodName = new WCHAR[_authMethod.length()+1];
        _authMethod.toWCharArray(methodName);
        methodName[_authMethod.length()] = 0;
        status = QuerySecurityPackageInfo(methodName, &_info);
        TimeStamp serverLifetime;
        status = AcquireCredentialsHandle(NULL, methodName, SECPKG_CRED_BOTH, NULL, NULL, NULL, NULL, &_serverHandle, &serverLifetime);

        _outSecBufDesc.ulVersion = 0;
        _outSecBufDesc.cBuffers = 1;
        _outSecBufDesc.pBuffers = &_outSecBuf;
        _outSecBuf.cbBuffer = _info->cbMaxToken;
        _outSecBuf.BufferType = SECBUFFER_TOKEN;
        _outSecBuf.pvBuffer = (PBYTE) malloc (_info->cbMaxToken);
        _inSecBufDesc.ulVersion = 0;
        _inSecBufDesc.cBuffers = 1;
        _inSecBufDesc.pBuffers = &_inSecBuf;
        _inSecBuf.BufferType = SECBUFFER_TOKEN;
        delete methodName;
#endif
    }
    ~GSSAPISession()
    {
#ifdef Q_OS_WIN
        free(_outSecBuf.pvBuffer);
#endif
    }
    QVariant securityToken() const
    {
#ifdef Q_OS_WIN
        return QVariant::fromValue<void*>(_secToken);
#endif
#ifdef Q_OS_UNIX
        throw "not implemented";
        return QVariant();
#endif
    }

    AUTH_RESULT authenticate()
    {
#ifdef Q_OS_WIN
        SECURITY_STATUS status;
        _inSecBuf.cbBuffer = inBuffer.length();
        _inSecBuf.BufferType = SECBUFFER_TOKEN;
        _inSecBuf.pvBuffer = inBuffer.data();
        _outSecBuf.cbBuffer = _info->cbMaxToken;

        CtxtHandle* contextPtr = &_context;
        if(_first)
        {
            contextPtr = NULL;
            _first = false;
        }
        status = AcceptSecurityContext (
                        &_serverHandle,
                        contextPtr,
                        &_inSecBufDesc,
                        0,
                        SECURITY_NATIVE_DREP,
                        &_context,
                        &_outSecBufDesc,
                        &_attrs,
                        &_serviceLifetime
                        );


        outBuffer = QByteArray((const char *)_outSecBuf.pvBuffer, _outSecBuf.cbBuffer);
        if(status == SEC_I_CONTINUE_NEEDED || status == SEC_I_COMPLETE_AND_CONTINUE) return AUTH_RESULT::CONTINUE;
        if(status == 0)
        {

            status = QuerySecurityContextToken(&_context, &_secToken);
            status  = QueryContextAttributes(&_context, SECPKG_ATTR_NAMES, &_names);
            return AUTH_RESULT::ACCEPTED;
        }

        return AUTH_RESULT::REJECTED;
#endif
#ifdef Q_OS_LINUX
        throw "not implemented";
        return AUTH_RESULT::REJECTED;
#endif
    }

};

GSSAPIAuthenticator::GSSAPIAuthenticator(QObject *parent) : Authenticator(parent), d_ptr(new GSSAPIAuthenticatorPrivate())
{

}
GSSAPIAuthenticator::~GSSAPIAuthenticator()
{

}

QString GSSAPIAuthenticator::authMethod() const
{
    Q_D(const GSSAPIAuthenticator);
    return d->_authMethod;
}
void GSSAPIAuthenticator::setAuthMethod(const QString &value)
{
    Q_D(GSSAPIAuthenticator);
    d->_authMethod = value;
}
QVariantMap GSSAPIAuthenticator::generateChallenge(qulonglong sessionId, QString authId)
{
    Q_UNUSED(sessionId);
    Q_UNUSED(authId);
    return QVariantMap();
}
AuthSession* GSSAPIAuthenticator::createSession() const
{
    Q_D(const GSSAPIAuthenticator);
    return new GSSAPISession(d->_authMethod);
}
User* GSSAPIAuthenticator::getUser(AuthSession *session)
{
    GSSAPISession* gssSession = static_cast<GSSAPISession*>(session);
    if(!gssSession) return NULL;
    for(User* user: _users.values())
    {
        QFlow::SIDUser* sid = static_cast<QFlow::SIDUser*>(user);
        if(!sid) continue;
        bool isMember = sid->checkTokenMembership(gssSession->securityToken());
        if(isMember)
        {
            return user;
        }
    }
    return NULL;
}
}
