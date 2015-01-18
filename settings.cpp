#include "settings.h"
#include <QJsonObject>
#include <QStandardPaths>

const auto litDoubleSavingsTransaction = QStringLiteral("doubleSavingsTransactions");
const auto litUsePinDateAsTransactionDate = QStringLiteral("pinDateAsTransactionDate");
const auto litScanDirectory = QStringLiteral("scanDirectory");

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    m_scanDirectory = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();
}

Settings *Settings::instance()
{
    static Settings settings;
    return &settings;
}

QJsonValue Settings::save() const
{
    QJsonObject settings;
    settings[litDoubleSavingsTransaction] = m_doubleSavingsTransactions;
    settings[litUsePinDateAsTransactionDate] = m_pinDateAsTransactionDate;
    settings[litScanDirectory] = m_scanDirectory;

    return QJsonValue(settings);
}

bool Settings::load(QJsonValue json)
{
    if (!json.isObject())
        return false;

    QJsonObject settings = json.toObject();
    m_doubleSavingsTransactions = settings.value(litDoubleSavingsTransaction).toBool(true);
    m_pinDateAsTransactionDate = settings.value(litUsePinDateAsTransactionDate).toBool(true);
    m_scanDirectory = settings.value(litScanDirectory).toString(m_scanDirectory);

    return true;
}

bool Settings::doubleSavingsTransactions() const
{
    return m_doubleSavingsTransactions;
}

bool Settings::pinDateAsTransactionDate() const
{
    return m_pinDateAsTransactionDate;
}

QString Settings::scanDirectory() const
{
    return m_scanDirectory;
}

bool Settings::saveSettingsOnExit() const
{
    return m_saveSettingsOnExit;
}

void Settings::setDoubleSavingsTransactions(bool doubleSavingsTransactions)
{
    if (m_doubleSavingsTransactions == doubleSavingsTransactions)
        return;

    m_doubleSavingsTransactions = doubleSavingsTransactions;
    emit doubleSavingsTransactionsChanged(doubleSavingsTransactions);
}

void Settings::setPinDateAsTransactionDate(bool pinDateAsTransactionDate)
{
    if (m_pinDateAsTransactionDate == pinDateAsTransactionDate)
        return;

    m_pinDateAsTransactionDate = pinDateAsTransactionDate;
    emit pinDateAsTransactionDateChanged(pinDateAsTransactionDate);
}

void Settings::setScanDirectory(QString arg)
{
    if (m_scanDirectory == arg)
        return;

    m_scanDirectory = arg;
    emit scanDirectoryChanged(arg);
}

void Settings::setSaveSettingsOnExit(bool arg)
{
    if (m_saveSettingsOnExit == arg)
        return;

    m_saveSettingsOnExit = arg;
    emit saveSettingsOnExitChanged(arg);
}

