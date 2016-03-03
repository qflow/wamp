#include "sid.h"
#include "user_p.h"
#include <QMutex>
#include <QDebug>
#include <atomic>

#ifdef Q_OS_WIN
#include <Windows.h>
#define SECURITY_WIN32
#include <Security.h>
#endif

namespace QFlow{

class SIDUserPrivate : public UserPrivate
{
public:
    std::atomic<bool> _initialized;
#ifdef Q_OS_WIN
    PSID _sid;
#endif
    SIDUserPrivate() : UserPrivate(), _initialized(false)
    {

    }
    ~SIDUserPrivate()
    {
        //FreeSid(_sid);
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
ErrorInfo SIDUser::init()
{
    Q_D(SIDUser);
    BOOL b;
    d->_sid = new SID();
    DWORD cbSid = sizeof(SID);
    DWORD cchReferencedDomainName = 0;
    SID_NAME_USE sidNameUse;
    LPCTSTR s = (LPCTSTR)name().utf16();
    LPTSTR refDomain = NULL;

    b = LookupAccountName(NULL,
                               s,
                               d->_sid, &cbSid, refDomain, &cchReferencedDomainName, &sidNameUse);
    SecureZeroMemory(d->_sid, cbSid);
    refDomain = new TCHAR[cchReferencedDomainName];
    b = LookupAccountName(NULL,
                               s,
                               d->_sid, &cbSid, refDomain, &cchReferencedDomainName, &sidNameUse);
    return ErrorInfo();
}

bool SIDUser::checkTokenMembership(QVariant handle)
{
#ifdef Q_OS_WIN
    Q_D(SIDUser);
    BOOL b;
    HANDLE accessHandle = handle.value<void*>();
    BOOL isMember;
    b = CheckTokenMembership(accessHandle, d->_sid, &isMember);
    return isMember;
#endif
#ifdef Q_OS_LINUX
    Q_UNUSED(handle);
    throw "not implemented";
    return false;
#endif
}
}
