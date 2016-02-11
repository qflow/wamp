#ifndef WAMP_H
#define WAMP_H

#include <QObject>
#include <QtPlugin>
#include <QQmlExtensionPlugin>

namespace QFlow{

class Wamp: public QQmlExtensionPlugin
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "Wamp" FILE "wamp_metadata.json")
public:
        void registerTypes(const char *uri);
};
}

#endif // WAMP_H
