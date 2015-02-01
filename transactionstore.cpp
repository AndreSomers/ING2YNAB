#include "transactionstore.h"
#include "transaction.h"
#include "transactionmodel.h"
#include <QFile>
#include <QJsonObject>
#include <QSet>
#include "csv.h"
#include <QJsonArray>
#include <QDebug>

class StringHashModel;


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


class StringHashModel: public QAbstractTableModel
{
public:
    StringHashModel(StringHash&data, QObject* parent);
    void setColumnHeaders(const QString& header0,
                          const QString& header1);
    void setNewItemTemplate(const QString& leftItem,
                            const QString& rightItem);

protected:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    StringHash& m_data;
    QList<QString> m_keys;
    QString m_header0;
    QString m_header1;
    QString m_newItemTemplate0;
    QString m_newItemTemplate1;
};


StringHashModel::StringHashModel(StringHash &data, QObject *parent):
    QAbstractTableModel(parent),
    m_data(data),
    m_keys(data.keys())
{
    std::sort(m_keys.begin(), m_keys.end());
}

void StringHashModel::setColumnHeaders(const QString &header0, const QString &header1)
{
    if (header0 != m_header0) {
        m_header0 = header0;
        emit headerDataChanged(Qt::Horizontal, 0, 0);
    }
    if (header1 != m_header1) {
        m_header1 = header1;
        emit headerDataChanged(Qt::Horizontal, 1, 1);
    }
}

void StringHashModel::setNewItemTemplate(const QString &leftItem, const QString &rightItem)
{
    m_newItemTemplate0 = leftItem;
    m_newItemTemplate1 = rightItem;
}

int StringHashModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_data.count();
}

int StringHashModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant StringHashModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid() ||
        index.row() < 0 ||
        index.row() >= rowCount(index.parent()) ||
        index.column() < 0 ||
        index.column() >= columnCount(index.parent()))
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto key = m_keys.at(index.row());
        if (index.column() == 0) {
            return key;
        } else if (index.column() == 1){
            return m_data.value(key);
        }
    }

    return QVariant();
}

QVariant StringHashModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0: return m_header0;
        case 1: return m_header1;
        default: return QVariant();
        }
    }

    return QVariant();
}

bool StringHashModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.parent().isValid() ||
        index.row() < 0 ||
        index.row() >= rowCount(index.parent()) ||
        index.column() < 0 ||
        index.column() >= columnCount(index.parent()))
    {
        return false;
    }

    QVector<int> roles { Qt::DisplayRole, Qt::EditRole};
    if (roles.contains(role)) {
        auto key = m_keys.at(index.row());
        auto origValue = m_data.value(key);
        if (index.column()==0 ) {
            m_data.remove(key);
            m_data.insert(value.toString(), origValue);
            m_keys[index.row()] = value.toString();
            emit dataChanged(index, index, roles);
        } else if (index.column() ==1) {
            m_data.insert(key, value.toString());
            emit dataChanged(index, index, roles);
        }
    }

    return true;
}

Qt::ItemFlags StringHashModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid() ||
        index.row() < 0 ||
        index.row() >= rowCount(index.parent()) ||
        index.column() < 0 ||
        index.column() >= columnCount(index.parent()))
    {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

bool StringHashModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() ||
        row < 0 ||
        row > rowCount(parent))
    {
        return false;
    }

    QString newItemTextLeft = m_newItemTemplate0.arg("").trimmed();
    int i(0);
    while(m_keys.contains(newItemTextLeft)) {
        newItemTextLeft = m_newItemTemplate0.arg(++i);
    }

    beginInsertRows(parent, row, row);
    m_keys.insert(row, newItemTextLeft);
    m_data.insert(newItemTextLeft, m_newItemTemplate1);
    endInsertRows();

    if (--count > 0) {
        return insertRows(row+1, count, parent);
    }

    return true;
}

bool StringHashModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid() ||
        row < 0 ||
        row + count > rowCount(parent))
    {
        return false;
    }

    beginRemoveRows(parent,row, row + count - 1);
    auto itBegin = m_keys.begin() + row;
    auto itEnd = itBegin + count;

    std::for_each(itBegin, itEnd, [&](const QString& key) {m_data.remove(key);});
    m_keys.erase(itBegin, itEnd);

    endRemoveRows();

    return true;
}

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

QAbstractItemModel *TransactionStore::accountNameModel()
{
    auto model = new StringHashModel(m_accountNameMap, this);
    model->setColumnHeaders(tr("Account"),
                            tr("Name"));
    model->setNewItemTemplate(tr("New account %1"),
                              tr("Name"));
    return model;
}

QAbstractItemModel *TransactionStore::cardNameModel()
{
    auto model = new StringHashModel(m_cardNameMap, this);
    model->setColumnHeaders(tr("Card"),
                            tr("Name"));
    model->setNewItemTemplate(tr("New card %1"),
                              tr("Name"));
    return model;
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


