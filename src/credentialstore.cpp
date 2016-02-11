#include "credentialstore.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <wincred.h>
#endif

#ifdef Q_OS_LINUX
#define SECRET_API_SUBJECT_TO_CHANGE
#include <libsecret/secret.h>
#endif

namespace QFlow{


CredentialStore::CredentialStore(QObject *parent) : QObject(parent)
{
}
CredentialStore::~CredentialStore()
{

}

#ifdef Q_OS_LINUX
GList* getItems(QString credential)
{
    GError *error = NULL;
    GHashTable* attrs = g_hash_table_new(NULL, NULL);
    g_hash_table_insert(attrs, (gpointer)"url", g_strdup(credential.toUtf8().constData()));

    GList* items = secret_service_search_sync(NULL, // default secret service
                                              NULL, attrs,
                                              SECRET_SEARCH_NONE,
                                              NULL, // no cancellable ojbect
                                              &error);
    return items;
}
#endif


QString CredentialStore::readPassword(QUrl resource)
{
#ifdef Q_OS_WIN
    PCREDENTIALW pcred;
    BOOL ok = CredReadW((LPCWSTR)resource.url().data(), CRED_TYPE_GENERIC, 0, &pcred);
    if(ok) return QString((const QChar*)pcred->CredentialBlob);
    return QString();
#endif
#ifdef Q_OS_LINUX
    GList *items = getItems(resource.url());
    if(!items) return QString();
    GList* first = g_list_first (items);
    SecretItem *item = (SecretItem*)first->data;
    GError *error = NULL;
    secret_item_load_secret_sync (item, NULL, &error);
    SecretValue* secret = secret_item_get_secret (item);
    const gchar *at;
    gsize length;
    at = secret_value_get (secret, &length);
    QByteArray arr(at, length);
    g_list_free_full (items, g_object_unref);
    return QString(arr);
#endif
}
QString CredentialStore::readUsername(QUrl resource)
{
#ifdef Q_OS_WIN
    PCREDENTIALW pcred;
    BOOL ok = CredReadW((LPCWSTR)resource.url().data(), CRED_TYPE_GENERIC, 0, &pcred);
    if(ok) return QString((const QChar*)pcred->UserName);
    return QString();
#endif
#ifdef Q_OS_LINUX
    GList *items = getItems(resource.url());
    if(!items) return QString();
    GList* first = g_list_first (items);
    SecretItem *item = (SecretItem*)first->data;
    GHashTable* attributes = secret_item_get_attributes (item);
    const char* userC = (const char*)g_hash_table_lookup(attributes, (gpointer)"user");
    QString user(userC);
    g_hash_table_unref (attributes);
    g_list_free_full (items, g_object_unref);
    return user;
#endif
}
}
