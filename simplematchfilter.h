#ifndef SIMPLEMATCHFILTER_H
#define SIMPLEMATCHFILTER_H
#include "abstractfilter.h"
#include <functional>

class SimpleMatchFilter : public AbstractFilter
{
public:
    SimpleMatchFilter(QObject* parent = 0);
    ~SimpleMatchFilter();

    bool load(QJsonValue json) override;
    QJsonValue save() const override;

    bool filter(Transaction* transaction) override;

private:
    using Strategy = std::function<bool(bool, bool)>;
    using Comparison = std::function<bool(QVariant, QVariant)>;
    enum class Operator {NoOp, SetValue};

    struct Condition {
        QByteArray property;
        Comparison comparison;
        QString value;
    };

    struct Action {
        QByteArray property;
        Operator op;
        QString value;
    };

private:
    bool evaluateCondition(const Condition& condition, Transaction* transaction);
    bool applyAction(const Action& action, Transaction* transaction);
    
private:
    Strategy m_strategy;
    QList<Condition> m_conditions;
    QList<Action> m_actions;
};

#endif // SIMPLEMATCHFILTER_H
