#ifndef TRANSACTIONMODELFILTER_H
#define TRANSACTIONMODELFILTER_H
#include <QSortFilterProxyModel>

class TransactionModelFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TransactionModelFilter(QObject* parent = 0);
    ~TransactionModelFilter();

public slots:
    void setFilterString(QString filter);

protected:
//    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QString m_filter;
};

#endif // TRANSACTIONMODELFILTER_H
