#ifndef CREDENTIALSTORE_H
#define CREDENTIALSTORE_H

#include <QObject>
#include <QUrl>

namespace QFlow{

class CredentialStore : public QObject
{
    Q_OBJECT
public:
    explicit CredentialStore(QObject *parent = 0);
    ~CredentialStore();
    QString readPassword(QUrl resource);
    QString readUsername(QUrl resource);
Q_SIGNALS:

public Q_SLOTS:
};
}
#endif // CREDENTIALSTORE_H
