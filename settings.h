#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QJsonValue>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool doubleSavingsTransactions READ doubleSavingsTransactions WRITE setDoubleSavingsTransactions NOTIFY doubleSavingsTransactionsChanged)
    Q_PROPERTY(bool pinDateAsTransactionDate READ pinDateAsTransactionDate WRITE setPinDateAsTransactionDate NOTIFY pinDateAsTransactionDateChanged)
    Q_PROPERTY(QString scanDirectory READ scanDirectory WRITE setScanDirectory NOTIFY scanDirectoryChanged)
    Q_PROPERTY(bool saveSettingsOnExit READ saveSettingsOnExit WRITE setSaveSettingsOnExit NOTIFY saveSettingsOnExitChanged)

public:
    explicit Settings(QObject *parent = 0);
    ~Settings() = default;
    Settings(const Settings& other) = delete;
    Settings& operator=(const Settings& rhs) = delete;

    static Settings* instance();

    QJsonValue save() const;
    bool load(QJsonValue json);

    bool doubleSavingsTransactions() const;
    bool pinDateAsTransactionDate() const;

    QString scanDirectory() const;

    bool saveSettingsOnExit() const;

signals:
    void doubleSavingsTransactionsChanged(bool arg);
    void pinDateAsTransactionDateChanged(bool arg);

    void scanDirectoryChanged(QString arg);

    void saveSettingsOnExitChanged(bool arg);

public slots:
    void setDoubleSavingsTransactions(bool doubleSavingsTransactions);
    void setPinDateAsTransactionDate(bool pinDateAsTransactionDate);

    void setScanDirectory(QString arg);

    void setSaveSettingsOnExit(bool arg);

private:
    bool m_doubleSavingsTransactions = true;
    bool m_pinDateAsTransactionDate = true;
    QString m_scanDirectory;
    bool m_saveSettingsOnExit = false;
};

#endif // SETTINGS_H
