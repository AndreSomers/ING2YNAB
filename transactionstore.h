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
    QAbstractItemModel* accountNameModel();
    QAbstractItemModel* cardNameModel();

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



#endif // TRANSACTIONSTORE_H
