#include "simplematchfilter.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

SimpleMatchFilter::SimpleMatchFilter(QObject *parent):
    AbstractFilter(parent)
{

}

SimpleMatchFilter::~SimpleMatchFilter()
{

}

bool SimpleMatchFilter::load(QJsonValue json)
{

}

QJsonValue SimpleMatchFilter::save() const
{
/*
    QJsonValue ret = AbstractFilter::save();
    QJsonObject object = ret.toObject();
    object["combine"] = m_strategy == Stragegy::And ? QStringLiteral("And") : QStringLiteral("Or");

    QJsonArray conditions;
    for(const auto& cond : m_conditions) {
        QJsonObject conditionObject;
        conditionObject["property"] = cond.property;
        conditionObject["value"] = cond.value;
        conditionObject["comparison"] = cond.comparison;

        conditions.append(conditionObject);
    }
    object["conditions"] = conditions;

    QJsonObject actions;
    for(const auto& act : m_actions) {
        QJsonObject actionObject;
        actionObject["property"] = act.property;
        actionObject["value"] = act.value;
        actionObject["operator"] = act.op;

        actions.append(actionObject);
    }
    object["actions"] = actions;

    object["type"] = QStringLiteral("match");
    return QJsonValue(object);
    */
}

bool SimpleMatchFilter::filter(Transaction *transaction)
{

}

