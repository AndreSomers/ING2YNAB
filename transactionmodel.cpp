#include "transactionmodel.h"
#include "transaction.h"

TransactionModel::TransactionModel(TransactionStore *store):
    QAbstractTableModel(store),
    m_store(store)
{
    Q_ASSERT(store);
    QHash<int, QVariant> emptyRole;
    QHash<int, QVariant> alignmentRole;
    alignmentRole.insert(Qt::TextAlignmentRole, Qt::AlignRight);

    m_columns << ColumnData{tr("Date"), "date", emptyRole}
              << ColumnData{tr("Name"), "name", emptyRole}
              << ColumnData{tr("Account"), "accountName", emptyRole}
              << ColumnData{tr("Category"), "category", emptyRole}
              << ColumnData{tr("Other Account"), "otherAccountName", emptyRole}
              << ColumnData{tr("Type"), "typeName", emptyRole}
              << ColumnData{tr("In"), "mutationIn", alignmentRole}
              << ColumnData{tr("Out"), "mutationOut", alignmentRole}
              << ColumnData{tr("Description"), "description", emptyRole}
              << ColumnData{tr("Card"), "cardName", emptyRole}
              << ColumnData{tr("Time stamp"), "timeStamp", emptyRole}
              << ColumnData{tr("Recurring"), "recurring", emptyRole}
              << ColumnData{tr("Sepa ID"), "sepa", emptyRole}
              << ColumnData{tr("BIC"), "bic", emptyRole}
              << ColumnData{tr("Transaction ID"), "transactionId", emptyRole}
              << ColumnData{tr("Full description"), "fullDescription", emptyRole};
}

TransactionModel::~TransactionModel()
{

}

int TransactionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_store->count();
}

int TransactionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_columns.count();
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 ||
        index.row() >= rowCount(index.parent()) ||
        index.column() < 0 ||
        index.column() >= columnCount(index.parent()) ||
        index.parent().isValid())
    {
        return QVariant();
    }

    Transaction* transaction = m_store->transaction(index.row());
    QByteArray propertyName = m_columns[index.column()].propertyName;

    if (role == Qt::DisplayRole) {
        return transaction->property(propertyName);
    } else if (role == Qt::EditRole) {
        if (index.column() == 6 || index.column() == 7) {
            return transaction->mutationCents();
        } else {
            return transaction->property(propertyName);
        }
    } else {
        return m_columns[index.column()].roles.value(role, QVariant());
    }

    return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();

    if (section < 0 || section >= columnCount(QModelIndex())) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return m_columns[section].name;
    }

    return QVariant();
}

