#ifndef FILTERENGINE_H
#define FILTERENGINE_H

#include <QObject>
#include <QJsonValue>

class Transaction;
class AbstractFilter;

class FilterEngine : public QObject
{
    Q_OBJECT
public:
    explicit FilterEngine(QObject *parent = 0);
    ~FilterEngine();

    bool load(QJsonValue json);
    QJsonValue save() const;

    void append(AbstractFilter *filter);

signals:
    void filterAdded(AbstractFilter* filter);

public slots:
    void filter(Transaction* transaction);

private:
    QList<AbstractFilter*> m_filters;
};

#endif // FILTERENGINE_H
