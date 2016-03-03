#ifndef WAMPWORKER_H
#define WAMPWORKER_H

#include <QObject>
#include <QTimer>

namespace QFlow{

class WampConnectionPrivate;
class WampWorker : public QObject
{
    Q_OBJECT
public:
    WampWorker();
    ~WampWorker();
    WampConnectionPrivate* _socketPrivate;
    QTimer* _timer;
public Q_SLOTS:
    void connect();
    void messageReceived(const QByteArray & message);
    void opened();
    void closed();
    void sendTextMessage(const QString& message);
    void sendBinaryMessage(const QByteArray& message);
    void reconnect();
    void flush();
};
}
#endif // WAMPWORKER_H
