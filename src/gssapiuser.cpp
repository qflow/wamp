#include "gssapiuser.h"
#include <QByteArray>

#ifdef Q_OS_WIN
#include <Windows.h>
#define SECURITY_WIN32
#include <Security.h>
#endif

namespace QFlow{

class GSSAPIUserPrivate
{
public:
    QString _authMethod;
    bool _first;
#ifdef Q_OS_WIN
    CredHandle _handle;
    CtxtHandle _context;
    SecBufferDesc  _outSecBufDesc;
    SecBuffer _outSecBuf;
    SecBufferDesc _inSecBufDesc;
    SecBuffer _inSecBuf;
    PSecPkgInfo _info;
    ULONG _attrs;
    TimeStamp _serviceLifetime;
#endif
    GSSAPIUserPrivate() : _first(true)
    {

    }
    ~GSSAPIUserPrivate()
    {

    }
};

GSSAPIUser::GSSAPIUser(QObject *parent) : User(parent), d_ptr(new GSSAPIUserPrivate())
{

}
GSSAPIUser::~GSSAPIUser()
{

}
QString GSSAPIUser::authMethod() const
{
    Q_D(const GSSAPIUser);
    return d->_authMethod;
}
void GSSAPIUser::setAuthMethod(const QString &value)
{
    Q_D(GSSAPIUser);
    d->_authMethod = value;

#ifdef Q_OS_WIN
    SECURITY_STATUS status;

    LPWSTR methodName = new WCHAR[d->_authMethod.length()+1];
    d->_authMethod.toWCharArray(methodName);
    methodName[d->_authMethod.length()] = 0;
    status = QuerySecurityPackageInfo(methodName, &d->_info);
    TimeStamp lifetime;
    status = AcquireCredentialsHandle(NULL, methodName, SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL, &d->_handle, &lifetime);


    d->_outSecBufDesc.ulVersion = 0;
    d->_outSecBufDesc.cBuffers = 1;
    d->_outSecBufDesc.pBuffers = &d->_outSecBuf;
    d->_outSecBuf.cbBuffer = d->_info->cbMaxToken;
    d->_outSecBuf.BufferType = SECBUFFER_TOKEN;
    d->_outSecBuf.pvBuffer = (PBYTE) malloc (d->_info->cbMaxToken);

    d->_inSecBufDesc.ulVersion = 0;
    d->_inSecBufDesc.cBuffers = 1;
    d->_inSecBufDesc.pBuffers = &d->_inSecBuf;
    d->_inSecBuf.BufferType = SECBUFFER_TOKEN;

    SecPkgCredentials_Names names;
    status = QueryCredentialsAttributes(&d->_handle, SECPKG_CRED_ATTR_NAMES, &names);
    QString username((const QChar*)names.sUserName);
    setName(username);
#endif
}

QByteArray GSSAPIUser::response(QByteArray challenge)
{
#ifdef Q_OS_WIN
    Q_D(GSSAPIUser);
    SECURITY_STATUS status;
    d->_inSecBuf.cbBuffer = challenge.length();
    d->_inSecBuf.pvBuffer = challenge.data();
    d->_outSecBuf.cbBuffer = d->_info->cbMaxToken;
    CtxtHandle* ctxPointer = &d->_context;
    SecBufferDesc* inBufferPointer = &d->_inSecBufDesc;
    if(d->_first)
    {
        ctxPointer = NULL;
        inBufferPointer = NULL;
        d->_first = false;
    }

    status = InitializeSecurityContext (
                               &d->_handle,
                               ctxPointer,
                               NULL,
                               // context requirements
                               0,
                               0,      // reserved1
                               SECURITY_NATIVE_DREP,
                               inBufferPointer,
                               0,
                               &d->_context,
                               &d->_outSecBufDesc,
                               &d->_attrs,
                               &d->_serviceLifetime
                               );
    if(status == 0)
    {
        d->_first = true;
        d->_context = CtxtHandle();
    }
    QByteArray token((const char *)d->_outSecBuf.pvBuffer, d->_outSecBuf.cbBuffer);
    return token;
#endif
#ifdef Q_OS_LINUX
    Q_UNUSED(challenge);
    throw "not implemented";
    return QByteArray();
#endif
}
}
