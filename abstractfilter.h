#ifndef ABSTRACTFILTER_H
#define ABSTRACTFILTER_H

#include <QObject>
#include <QJsonValue>

class Transaction;

class AbstractFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit AbstractFilter(QObject *parent = 0);
    ~AbstractFilter();

    QString name() const {return m_name;}
    Q_SLOT void setName(QString name);
    Q_SIGNAL void nameChanged(QString name);

    virtual bool load(QJsonValue json);
    virtual QJsonValue save() const;

    virtual bool filter(Transaction* transaction) = 0;

signals:

public slots:

private:
    QString m_name;
};

class FilterFactory
{
public:
    static AbstractFilter* createFilter(QString type, QObject *parent = 0);
};

#endif // ABSTRACTFILTER_H
