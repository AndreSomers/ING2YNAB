#include "transactionstore.h"
#include "transaction.h"
#include "transactionmodel.h"
#include <QFile>
#include <QJsonObject>
#include <QSet>
#include "csv.h"
#include <QJsonArray>

TransactionStore::TransactionStore(QObject *parent) : QObject(parent)
{
    m_model = new TransactionModel(this);
}

TransactionStore::~TransactionStore()
{
}

void TransactionStore::clear()
{
    m_model->beginResetModel();
    qDeleteAll(m_transactions);
    m_files.clear();

    m_transactions.clear();
    m_model->endResetModel();

    updateSummaryValues();
}

void TransactionStore::addRawData(QList<QStringList> transactions, QString sourceFile)
{
    foreach(QStringList rawTransaction, transactions) {
        Transaction* transaction = new Transaction(rawTransaction, sourceFile, this);
        m_model->beginInsertRows(QModelIndex(),
                                 m_transactions.count(),
                                 m_transactions.count() + transaction->relatedTransactions().count() );
        m_transactions.append(transaction);
        emit transactionAdded(transaction);
        m_transactions.append(transaction->relatedTransactions());
        m_model->endInsertRows();
    }
    updateSummaryValues();
}

bool TransactionStore::setFromFile(const QString& fileName)
{
    clear();
    return addFromFile(fileName);
}

bool TransactionStore::addFromFile(const QString& fileName)
{
    QFile dataFile(fileName);
    if (dataFile.open(QFile::ReadOnly)) {
        QList<QStringList> rawData = CSV::parseFromFile(fileName);
        rawData.takeFirst(); //remove header row
        addRawData(rawData, fileName);
        m_files << fileName;
        return true;
    } else {
        return false;
    }
}

void TransactionStore::setAccountNameMap(StringHash accountNames)
{
    m_accountNameMap = accountNames;
}

void TransactionStore::setCardNameMap(StringHash cardNames)
{
    m_cardNameMap = cardNames;
}

QAbstractItemModel *TransactionStore::model()
{
    return m_model;
}

void TransactionStore::updateSummaryValues()
{
    m_fromDate = QDate();
    m_toDate = QDate();
    m_totalIn = 0;
    m_totalOut = 0;

    QSet<QString> accounts;

    foreach (Transaction* transaction, m_transactions) {
        if (m_fromDate.isNull() || m_fromDate > transaction->date()) {
            m_fromDate = transaction->date();
        }
        if (m_toDate.isNull() || m_toDate < transaction->date()) {
            m_toDate = transaction->date();
        }

        if (transaction->type() & Transaction::TTSavingsFlag) {
            m_totalSavings += qMax(qint64(0), transaction->mutationCents());
        } else {
            if (transaction->mutationCents() < 0) {
                m_totalOut -= transaction->mutationCents();
            } else {
                m_totalIn += transaction->mutationCents();
            }
        }

        accounts.insert(transaction->accountNumber());
    }

    m_accountCount = accounts.count();
}



QJsonValue StringHash::save() const
{
    QJsonArray result;

    QHashIterator<QString, QString> it(*this);
    while (it.hasNext()) {
        it.next();
        QJsonObject entry;
        entry["key"] = it.key();
        entry["value"] = it.value();
        result.append(entry);
    }

    return QJsonValue(result);
}

bool StringHash::load(QJsonValue json)
{
    if (!json.isArray()) {
        return false;
    }

    clear();
    QJsonArray array = json.toArray();
    for(const auto& val: array) {
        if (val.isObject()) {
            QJsonObject entry = val.toObject();
            insert(entry["key"].toString(), entry["value"].toString());
        }
    }

    return true;
}
