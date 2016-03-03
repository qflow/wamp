#include "wampcraauthenticator.h"
#include "random.h"
#include "wamproutersession.h"
#include <QMessageAuthenticationCode>
#include <QJsonDocument>
#include <QDateTime>

namespace QFlow{

class WampCraAuthenticatorPrivate
{
public:
    WampCraAuthenticatorPrivate()
    {

    }
    ~WampCraAuthenticatorPrivate()
    {

    }
};

class WampCraSession: public AuthSession
{
public:
    WampCraSession() : AuthSession()
    {

    }
    ~WampCraSession()
    {

    }
    AUTH_RESULT authenticate() override
    {
        QMessageAuthenticationCode hash(QCryptographicHash::Sha256);
        WampCraUser* crauser = (WampCraUser*)user.data();
        hash.setKey(crauser->secret().toLatin1());
        QString challengeStr = challenge["challenge"].toString();
        hash.addData(challengeStr.toLatin1());

        QByteArray hashResult = hash.result();
        QByteArray finalSig = hashResult.toBase64();
        if(finalSig == inBuffer) return AUTH_RESULT::ACCEPTED;
        return AUTH_RESULT::REJECTED;
    }
};

WampCraAuthenticator::WampCraAuthenticator(QObject *parent) : Authenticator(parent),
    d_ptr(new WampCraAuthenticatorPrivate())
{

}
WampCraAuthenticator::~WampCraAuthenticator()
{

}
QString WampCraAuthenticator::authMethod() const
{
    return "wampcra";
}
QVariantMap WampCraAuthenticator::generateChallenge(qulonglong sessionId, QString authId)
{
    QVariantMap retObj;
    QJsonObject challenge;
    challenge["authid"] = authId;
    challenge["authprovider"] = "wampcra";
    challenge["authmethod"] = "wampcra";
    qulonglong nonce = Random::generate();
    QString nonceStr = QString("%1").arg(nonce);
    challenge["nonce"] = nonceStr;
    QString timestampStr = QDateTime::currentDateTimeUtc().toString("yyyy-mm-ddThh:mm:zzzZ");
    challenge["timestamp"] = timestampStr;
    challenge["session"] = (double)sessionId;
    QJsonDocument challengeDoc(challenge);
    QString challengeStr = challengeDoc.toJson();
    retObj["challenge"] = challengeStr;
    return retObj;
}
AuthSession* WampCraAuthenticator::createSession() const
{
    return new WampCraSession();
}
User* WampCraAuthenticator::getUser(AuthSession *session)
{
    Q_UNUSED(session);
    throw "not implemented";
    return NULL;
}
}
