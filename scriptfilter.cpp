#include "scriptfilter.h"
#include <QJsonObject>
#include <QScriptEngine>
#include <QApplication>
#include "transaction.h"
#include <QDebug>

ScriptFilter::ScriptFilter(QObject *parent):
    AbstractFilter(parent)
{

}

ScriptFilter::~ScriptFilter()
{

}

bool ScriptFilter::load(QJsonValue json)
{
    if (!AbstractFilter::load(json))
        return false;

    QJsonObject object = json.toObject();
    m_scriptText = object["script"].toString();
    m_script = engine()->evaluate(m_scriptText, name());

    return true;
}

QJsonValue ScriptFilter::save() const
{
    QJsonValue ret = AbstractFilter::save();
    QJsonObject object = ret.toObject();
    object["script"] = m_scriptText;
    object["type"] = QStringLiteral("script");
    return QJsonValue(object);
}

bool ScriptFilter::filter(Transaction *transaction)
{
    QScriptValueList args;
    QScriptValue arg = engine()->newQObject(transaction,
                                            QScriptEngine::QtOwnership,
                                            ( QScriptEngine::ExcludeSuperClassContents |
                                              QScriptEngine::ExcludeDeleteLater |
                                              QScriptEngine::AutoCreateDynamicProperties) );
    args << arg;
    QScriptValue ret = m_script.call(QScriptValue(), args);

    if (engine()->hasUncaughtException()) {
        qDebug() << engine()->uncaughtException().toString();
    }

    return ret.toBool();
}

QScriptEngine *ScriptFilter::engine()
{
    QScriptEngine* engine = qApp->findChild<QScriptEngine*>();
    if (!engine) {
        engine = new QScriptEngine(qApp);
    }

    return engine;
}

