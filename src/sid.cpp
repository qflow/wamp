#include "sid.h"
#include "user_p.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#define SECURITY_WIN32
#include <Security.h>
#endif

namespace QFlow{

class SIDUserPrivate : public UserPrivate
{
public:
    bool _initialized;
#ifdef Q_OS_WIN
    SID _sid;
#endif
    SIDUserPrivate() : UserPrivate(), _initialized(false)
    {

    }
    ~SIDUserPrivate()
    {

    }
};

SIDUser::SIDUser(QObject *parent) : User(*new SIDUserPrivate, parent)
{

}
SIDUser::SIDUser(SIDUserPrivate &dd, QObject *parent) : User(dd, parent)
{

}

SIDUser::~SIDUser()
{

}
QByteArray SIDUser::response(QByteArray)
{
    return QByteArray();
}
QString SIDUser::authMethod() const
{
    return QString();
}
bool SIDUser::checkTokenMembership(QVariant handle)
{
#ifdef Q_OS_WIN
    Q_D(SIDUser);
    BOOL b;
    if(!d->_initialized)
    {
        DWORD cbSid = sizeof(d->_sid);
        DWORD cchReferencedDomainName = 0;
        SID_NAME_USE sidNameUse;
        LPCTSTR s = (LPCTSTR)name().utf16();
        LPTSTR refDomain = NULL;
        b = LookupAccountName(NULL,
                                   s,
                                   &d->_sid, &cbSid, refDomain, &cchReferencedDomainName, &sidNameUse);
        SecureZeroMemory(&d->_sid, cbSid);
        refDomain = new TCHAR[cchReferencedDomainName];
        b = LookupAccountName(NULL,
                                   s,
                                   &d->_sid, &cbSid, refDomain, &cchReferencedDomainName, &sidNameUse);
    }
    HANDLE accessHandle = handle.value<void*>();
    BOOL isMember;
    b = CheckTokenMembership(accessHandle, &d->_sid, &isMember);
    return isMember;
#endif
#ifdef Q_OS_LINUX
    Q_UNUSED(handle);
    throw "not implemented";
    return false;
#endif
}
}
