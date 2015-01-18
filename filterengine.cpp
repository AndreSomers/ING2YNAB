#include "filterengine.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "scriptfilter.h"
#include "abstractfilter.h"

FilterEngine::FilterEngine(QObject *parent) : QObject(parent)
{

}

FilterEngine::~FilterEngine()
{

}

bool FilterEngine::load(QJsonValue json)
{
    if (!json.isArray())
        return false;

    QJsonArray filterItems = json.toArray();
    for(const auto& filterItem: filterItems) {
        if (filterItem.isObject()) {
            QJsonObject filterObject = filterItem.toObject();
            auto filter = FilterFactory::createFilter(filterObject["type"].toString(),
                                                      this);
            if (filter) {
                filter->load(filterItem);
                append(filter);
                qDebug() << "appended filter" << filter->name() << "to filter engine";
            }
        }
    }

    return true;
}

QJsonValue FilterEngine::save() const
{
    QJsonArray output;

    for(auto filter: m_filters) {
        output.append(filter->save());
    }

    return QJsonValue(output);
}

void FilterEngine::append(AbstractFilter *filter)
{
    filter->setParent(this);
    m_filters.append(filter);
    emit filterAdded(filter);
}

void FilterEngine::filter(Transaction *transaction)
{
    foreach(auto filter, m_filters) {
        if (filter->filter(transaction)) {
            break;
        }
    }
}

