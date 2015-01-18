#include "abstractfilter.h"
#include "scriptfilter.h"
#include "simplematchfilter.h"

#include <QJsonObject>

AbstractFilter::AbstractFilter(QObject *parent) :
    QObject(parent)
{

}

AbstractFilter::~AbstractFilter()
{

}

void AbstractFilter::setName(QString name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

bool AbstractFilter::load(QJsonValue json)
{
    if (!json.isObject())
        return false;

    QJsonObject object = json.toObject();
    setName(object["name"].toString());
    return true;
}

QJsonValue AbstractFilter::save() const
{
    QJsonObject object;
    object["name"] = name();

    return QJsonValue(object);
}



AbstractFilter *FilterFactory::createFilter(QString type, QObject* parent)
{
    if (type == "script") {
        return new ScriptFilter(parent);
    } else if (type == "match") {
        return new SimpleMatchFilter(parent);
    }

    return nullptr;
}
