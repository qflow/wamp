#ifndef WAMPATTACHED_H
#define WAMPATTACHED_H

#include <QObject>

namespace QFlow{


class WampAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRemote READ isRemote WRITE setIsRemote NOTIFY isRemoteChanged)
public:
    explicit WampAttached(QObject *parent = 0);
    virtual ~WampAttached();
    bool isRemote() const;
    void setIsRemote(bool value);
Q_SIGNALS:
    void isRemoteChanged();
private:
    bool _isRemote;
};
}
#endif // WAMPATTACHED_H
