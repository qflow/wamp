#include "wampattached.h"

namespace QFlow{
WampAttached::WampAttached(QObject *parent) : QObject(parent), _isRemote(true)
{

}
WampAttached::~WampAttached()
{

}
bool WampAttached::isRemote() const
{
    return _isRemote;
}
void WampAttached::setIsRemote(bool value)
{
    _isRemote = value;
}
}
