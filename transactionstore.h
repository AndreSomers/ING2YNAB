#ifndef TRANSACTIONSTORE_H
#define TRANSACTIONSTORE_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QDate>
#include <QJsonValue>
#include <QStringList>
#include <QAbstractTableModel>

class QAbstractItemModel;
class Transaction;
class TransactionModel;

class StringHash: public QHash<QString, QString>
{
public:
    StringHash() {}

    QJsonValue save() const;
    bool load(QJsonValue json);
};

class TransactionStore : public QObject
{
    Q_OBJECT
public:
    explicit TransactionStore(QObject *parent = 0);
    ~TransactionStore();
    void clear();
    void addRawData(QList<QStringList> transactions, QString sourceFile = QString());
    bool setFromFile(const QString& fileName);
    bool addFromFile(const QString& fileName);
    int count() const {return m_transactions.count();}
    Transaction* transaction(int index) {return m_transactions[index];}

    StringHash accountNameMap() const {return m_accountNameMap;}
    void setAccountNameMap(StringHash accountNames);
    StringHash cardNameMap() const {return m_cardNameMap;}
    void setCardNameMap(StringHash cardNames);

    QAbstractItemModel* model();

    QDate fromDate() const {return m_fromDate;}
    QDate toDate() const {return m_toDate;}
    qint64 totalIn() const {return m_totalIn;}
    qint64 totalOut() const {return m_totalOut;}
    qint64 totalSavings() const {return m_totalSavings;}
    int accountCount() const {return m_accountCount;}
    int fileCount() const {return m_files.count();}

signals:
    void transactionAdded(Transaction* transaction);

public slots:

private:
    void updateSummaryValues();

private:
    QList<Transaction*> m_transactions;
    TransactionModel* m_model;
    StringHash m_accountNameMap;
    StringHash m_cardNameMap;

    QDate m_fromDate;
    QDate m_toDate;
    qint64 m_totalIn;
    qint64 m_totalOut;
    qint64 m_totalSavings;
    int m_accountCount;
    QStringList m_files;

    friend class TransactionModel;
};

class StringHashModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    StringHashModel(StringHash&data, QObject* parent);
    void setColumnHeaders(const QString& header0,
                          const QString& header1);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    StringHash& m_data;
    QList<QString> m_keys;
    QString m_header0;
    QString m_header1;
};

#endif // TRANSACTIONSTORE_H
