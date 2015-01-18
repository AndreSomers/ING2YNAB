#ifndef CSV_H
#define CSV_H

#include <QStringList>

namespace CSV
{
    QList<QStringList> parseFromString(const QString &string,
                                       const QChar separator = QChar(','));
    QList<QStringList> parseFromFile(const QString &filename,
                                     const QChar separator = QChar(','),
                                     const QString &codec = "");
    bool write(const QList<QStringList> data,
               const QString &filename,
               const QChar separator = QChar(','),
               const QString &codec = "");
}

#endif // CSV_H
