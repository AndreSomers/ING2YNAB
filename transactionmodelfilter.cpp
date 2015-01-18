#include "transactionmodelfilter.h"

TransactionModelFilter::TransactionModelFilter(QObject* parent):
    QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setDynamicSortFilter(true);
    setFilterKeyColumn(-1);
}

TransactionModelFilter::~TransactionModelFilter()
{

}

void TransactionModelFilter::setFilterString(QString filter)
{
    if (filter != m_filter) {
        m_filter = filter;
        setFilterWildcard(QString("*%1*").arg(filter));
        invalidateFilter();
    }
}

bool TransactionModelFilter::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (left.column() == 6) {
        return left.data(Qt::EditRole) < right.data(Qt::EditRole);
    } else if (left.column() == 7) {
        return left.data(Qt::EditRole) > right.data(Qt::EditRole);
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

//bool TransactionModelFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
//{

//}

