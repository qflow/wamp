#ifndef WAMPWORKER_H
#define WAMPWORKER_H

#include "websocketconnection.h"
#include <QObject>
#include <QTimer>
#include <memory>


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
    std::unique_ptr<WebSocketConnection> _socket;
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
