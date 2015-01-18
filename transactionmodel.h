#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include "transactionstore.h"

class TransactionModel : public QAbstractTableModel
{
public:
    TransactionModel(TransactionStore* store);
    ~TransactionModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    struct ColumnData {
        //int index;
        QString name;
        QByteArray propertyName;
        QHash<int, QVariant> roles;
    };

private:
    TransactionStore* m_store;
    QList<ColumnData> m_columns;

    friend class TransactionStore;
};

#endif // TRANSACTIONMODEL_H
