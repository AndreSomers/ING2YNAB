#ifndef SCRIPTFILTER_H
#define SCRIPTFILTER_H

#include "abstractfilter.h"

#include <QScriptValue>
class QScriptEngine;

class ScriptFilter : public AbstractFilter
{
    Q_OBJECT
public:
    ScriptFilter(QObject *parent = 0);
    ~ScriptFilter();

    bool load(QJsonValue json) override;
    QJsonValue save() const override;

    bool filter(Transaction* transaction) override;

signals:

public slots:

private:
    static QScriptEngine* engine();

private:
    QScriptValue m_script;
    QString m_scriptText;

};

#endif // SCRIPTFILTER_H
