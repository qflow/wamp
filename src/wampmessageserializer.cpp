#include "wampmessageserializer.h"
#include "wamp_symbols.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <msgpack.hpp>
#include <sstream>
#include <QDebug>
#include <QDateTime>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template<>
struct pack<QVariant> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, QVariant const& v) const {
        if((QMetaType::Type)v.type() == QMetaType::Bool)
        {
            o.pack(v.toBool());
        }
        else if((QMetaType::Type)v.type() == QMetaType::QString)
        {
            o.pack(v.toString().toStdString());
        }
        else if((QMetaType::Type)v.type() == QMetaType::Int)
        {
            o.pack_int(v.toInt());
        }
        else if((QMetaType::Type)v.type() == QMetaType::ULongLong)
        {
            o.pack_unsigned_long_long(v.toULongLong());
        }
        else if((QMetaType::Type)v.type() == QMetaType::Float)
        {
            o.pack_float(v.toFloat());
        }
        else if((QMetaType::Type)v.type() == QMetaType::Double)
        {
            o.pack_double(v.toDouble());
        }
        else if((QMetaType::Type)v.type() == QMetaType::QByteArray)
        {
            QByteArray arr = v.toByteArray();
            o.pack_bin(arr.length());
            o.pack_bin_body(arr.data(), arr.length());
        }
        else if((QMetaType::Type)v.type() == QMetaType::QVariantList ||
                (QMetaType::Type)v.type() == QMetaType::QStringList)
        {
            o.pack(v.toList());
        }
        else if((QMetaType::Type)v.type() == QMetaType::QVariantMap)
        {
            o.pack(v.toMap());
        }
        else if((QMetaType::Type)v.type() == QMetaType::QDateTime)
        {
            QString utcStr = v.toDateTime().toUTC().toString(Qt::ISODate);
            o.pack(utcStr.toStdString());
        }
        else if(v.canConvert<int>())
        {
            o.pack_int(v.toInt());
        }
        else
        {
            qWarning() << "Could not serialize " << v.typeName();
        }
        return o;
    }
};
template<>
struct pack<QVariantList> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, QVariantList const& arr) const {
        o.pack_array(arr.length());
        for(QVariant var: arr)
        {
            o.pack(var);
        }
        return o;
    }
};
template<>
struct pack<QVariantMap> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, QVariantMap const& map) const {
        o.pack_map(map.count());
        for(QString key: map.keys())
        {
            o.pack(key.toStdString());
            o.pack(map[key]);
        }
        return o;
    }
};
template<>
struct convert<QVariant> {
    msgpack::object const& operator()(msgpack::object const& o, QVariant& v) const {
        if(o.type == msgpack::type::BOOLEAN)
        {
            v = QVariant(o.as<bool>());
        }
        else if(o.type == msgpack::type::STR)
        {
            v = QVariant(o.as<QString>());
        }
        else if(o.type == msgpack::type::POSITIVE_INTEGER)
        {
            v = QVariant((quint64)o.via.u64);
        }
        else if(o.type == msgpack::type::NEGATIVE_INTEGER)
        {
            v = QVariant((qint64)o.via.i64);
        }
        else if(o.type == msgpack::type::FLOAT)
        {
            v = QVariant(o.via.f64);
        }
        else if(o.type == msgpack::type::BIN)
        {
            QByteArray ba;
            ba.append(o.via.bin.ptr, o.via.bin.size);
            v = QVariant(ba);
        }
        else if(o.type == msgpack::type::ARRAY)
        {
            v = QVariant(o.as<QVariantList>());
        }
        else if(o.type == msgpack::type::MAP)
        {
            v = QVariant(o.as<QVariantMap>());
        }
        else
        {
            qWarning() << "Could not deserialize " << o.type;
        }
        return o;
    }
};
template<>
struct convert<QVariantList> {
    msgpack::object const& operator()(msgpack::object const& o, QVariantList& v) const {
        QVariantList list;
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        for(uint i=0; i<o.via.array.size; i++)
        {
            QVariant var = o.via.array.ptr[i].as<QVariant>();
            list.append(var);
        }
        v = list;
        return o;
    }
};
template<>
struct convert<QString> {
    msgpack::object const& operator()(msgpack::object const& o, QString& v) const {
        if (o.type != msgpack::type::STR) throw msgpack::type_error();
        QString str = QString::fromStdString(o.as<std::string>());
        v = str;
        return o;
    }
};
template<>
struct convert<QVariantMap> {
    msgpack::object const& operator()(msgpack::object const& o, QVariantMap& v) const {
        QVariantMap map;
        if (o.type != msgpack::type::MAP) throw msgpack::type_error();
        for(uint i=0; i<o.via.map.size; i++)
        {
            msgpack::object_kv pair = o.via.map.ptr[i];
            QVariant value = pair.val.as<QVariant>();
            QString key = pair.key.as<QString>();
            map[key] = value;
        }
        v = map;
        return o;
    }
};
}
}
}

QVariant toBase64(const QVariant& var)
{
    if((QMetaType::Type)var.type() == QMetaType::QByteArray)
    {
        QByteArray ba = var.toByteArray();
        return QVariant(ba.toBase64());
    }
    else if((QMetaType::Type)var.type() == QMetaType::QVariantList)
    {
        QVariantList list = var.toList();
        for(int i=0;i<list.length();i++)
        {
            list[i] = toBase64(list[i]);
        }
        return QVariant(list);
    }
    else if((QMetaType::Type)var.type() == QMetaType::QVariantMap)
    {
        QVariantMap map = var.toMap();
        for(QString key: map.keys())
        {
            map[key] = toBase64(map[key]);
        }
        return QVariant(map);
    }
    return var;
}

namespace QFlow{

WampMessageSerializer::WampMessageSerializer(QObject *parent) : QObject(parent)
{

}
WampMessageSerializer::~WampMessageSerializer()
{

}
bool WampMessageSerializer::isBinary() const
{
    return false;
}
WampMessageSerializer* WampMessageSerializer::create(const QString &name)
{
    if(name == KEY_WAMP_JSON_SUB) return new JsonMessageSerializer();
    else if(name == KEY_WAMP_MSGPACK_SUB) return new MsgpackMessageSerializer();
    return nullptr;
}

JsonMessageSerializer::JsonMessageSerializer(QObject *parent) : WampMessageSerializer(parent)
{

}
JsonMessageSerializer::~JsonMessageSerializer()
{

}
QVariantList JsonMessageSerializer::deserialize(const QByteArray &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonArray arr = doc.array();
    return arr.toVariantList();
}
QByteArray JsonMessageSerializer::serialize(const QVariantList &arr)
{
    QVariant listVarBase64 = toBase64(QVariant(arr));
    QJsonArray jsonArr = QJsonArray::fromVariantList(listVarBase64.toList());
    QJsonDocument doc(jsonArr);
    QByteArray message = doc.toJson();
    return message;
}

MsgpackMessageSerializer::MsgpackMessageSerializer(QObject *parent) : WampMessageSerializer(parent)
{

}
MsgpackMessageSerializer::~MsgpackMessageSerializer()
{

}
QVariantList MsgpackMessageSerializer::deserialize(const QByteArray &message)
{
    msgpack::unpacked result;
    msgpack::unpack(result, message.data(), message.length());
    msgpack::object deserialized = result.get();
    QVariantList arr = deserialized.as<QVariantList>();
    return arr;
}
QByteArray MsgpackMessageSerializer::serialize(const QVariantList &arr)
{
    std::stringstream buffer;
    msgpack::pack(buffer, arr);
    QByteArray message = QByteArray::fromStdString(buffer.str());
    return message;
}
bool MsgpackMessageSerializer::isBinary() const
{
    return true;
}
}
