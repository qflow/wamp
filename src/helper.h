#ifndef HELPER_H
#define HELPER_H

#include <QVariant>
#include <QJSValue>

namespace QFlow{
class Helper
{
public:
    Helper();
    static QJSValue variantToJS(const QVariant& var, QJSEngine* engine);
private:
    static QJSEngine _engine;
};
}
#endif // HELPER_H
