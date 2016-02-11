#include "helper.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJSEngine>

namespace QFlow{
Helper::Helper()
{

}
QJSValue Helper::variantToJS(const QVariant& var, QJSEngine* engine)
{
    if(var.isNull() || !var.isValid()) return QJSValue(QJSValue::NullValue);
    if((QMetaType::Type)var.type() == QMetaType::QByteArray)
    {
        QByteArray ba = var.toByteArray();
        QJSValue v = engine->toScriptValue(var);
        return v;
        /*QJSValue arr = engine->newArray(ba.length());
        for(quint32 i=0; i<ba.length(); i++)
        {
            arr.setProperty(i, QJSValue(ba[i]));
        }
        return arr;*/
    }
    if(var.canConvert<int>()) return QJSValue(var.toInt());
    if(var.canConvert<uint>()) return QJSValue(var.toUInt());
    if(var.canConvert<double>()) return QJSValue(var.toDouble());
    if(var.canConvert<bool>()) return QJSValue(var.toBool());
    if(var.canConvert<QString>()) return QJSValue(var.toString());
    return QJSValue();
}
}
