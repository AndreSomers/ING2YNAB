#include "transaction.h"
#include "transactionstore.h"
#include "settings.h"
#include <QRegularExpression>

QString Transaction::nameForType(int type)
{
    QString result;
    switch (type & ~TTSavingsFlag) {
    case TTTransfer:
        result = tr("Transfer");
        break;
    case TTPinTransaction:
        result = tr("Pin");
        break;
    case TTCashWithdrawal:
        result = tr("Cash withdrawal");
        break;
    case TTCollection:
        result = tr("Collection");
        break;
    case TTInternet:
        result = tr("Internet");
        break;
    case TTMassTransfer:
        result = tr("Mass transfer");
        break;
    case TTOther:
    default:
        result = tr("Other");
        break;
    }

    if (type & TTSavingsFlag) {
        result += tr(" (Savings)");
    } else if (type & TTCreditCardFlag) {
        result += tr(" (CreditCard)");
    }

    return result;
}

Transaction::Transaction(QStringList rawRecord,
                         TransactionStore *parent) :
    Transaction(parent)
{
    parseRawRecord(rawRecord);
}

Transaction::Transaction(TransactionStore *parent):
    QObject(parent),
    m_store(parent),
    m_type(TTOther),
    m_recurring(false)
{
    Q_ASSERT(parent);
}

Transaction::~Transaction()
{

}

QString Transaction::typeName() const
{
    return nameForType(m_type);
}

QString Transaction::accountName() const
{
    return m_store->accountNameMap().value(m_accountNumber, m_accountNumber);
}

QString Transaction::otherAccountName() const
{
    return m_store->accountNameMap().value(m_otherAccountNumber, m_otherAccountNumber);

}

QString Transaction::cardName() const
{
    return m_store->cardNameMap().value(m_cardNumber, m_cardNumber);

}

void Transaction::setDescription(QString description)
{
    if (description != m_description) {
        m_description = description;
        emit descriptionChanged(description);
    }
}


void Transaction::parseRawRecord(QStringList rawRecord)
{
//  "Datum","Naam / Omschrijving","Rekening","Tegenrekening","Code","Af Bij","Bedrag (EUR)","MutatieSoort","Mededelingen"
    enum IngFieldOrder {
        Date, Name, Account, OtherAccount, Code, InOut, Amount, MutationType, Notes
    };

    m_date = QDate::fromString(rawRecord[Date], QStringLiteral("yyyyMMdd"));
    m_name = rawRecord[Name];
    m_accountNumber = rawRecord[Account];
    m_otherAccountNumber = rawRecord[OtherAccount];
    m_fullDescription = rawRecord[Notes];

    QString amount = rawRecord[Amount];
    amount.remove(QChar(',')); //we store the amount in cents internally
    m_mutationCents = amount.toLongLong();
    if (rawRecord[InOut] == QStringLiteral("Af")) {
        m_mutationCents *= -1;
    }

    m_type = TTOther;
    if (rawRecord[Code] == QStringLiteral("OV")) {
        m_type = TTTransfer;
        parseFullDescriptionTransfer();
    } else if (rawRecord[Code] == QStringLiteral("IC")) {
        m_type = TTCollection;
        parseFullDescriptionIncasso();
    } else  if (rawRecord[Code] == QStringLiteral("BA")) {
        m_type = TTPinTransaction;
        parseFullDescriptionPin();
    } else if (rawRecord[Code] == QStringLiteral("GT")) {
        m_type = TTInternet;
        parseFullDescriptionInternet();
    } else if (rawRecord[Code] == QStringLiteral("GM")) {
        m_type = TTCashWithdrawal;
        parseFullDescriptionCashWithdraw();
    } else if (rawRecord[Code] == QStringLiteral("DV")) {
        m_type = TTOther;
        parseCreditCardTransaction();
    } else if (rawRecord[Code] == QStringLiteral("VZ")) {
        m_type = TTMassTransfer;
        parseStandardFields();
    }
}

void Transaction::parseFullDescriptionTransfer()
{
    if (!m_fullDescription.isEmpty()) {
        parseStandardFields();
    } else {
        parseSavingsTransaction();
    }
}
void Transaction::parseStandardFields()
{
    QStringList keywords{
      QStringLiteral("IBAN:"),                  //0
      QStringLiteral("BIC:"),                   //1
      QStringLiteral("Naam:" ),                 //2
      QStringLiteral("ID begunstigde:"),        //3
      QStringLiteral("SEPA ID machtiging:"),    //4
      QStringLiteral("Kenmerk:"),               //5
      QStringLiteral("Omschrijving:")};         //6

    int from = m_fullDescription.indexOf(keywords.first());
    if (from < 0)
        return;
    from += keywords.first().length();

    QStringList fields;
    for (int i=1; i<keywords.count(); i++) {
        int to = m_fullDescription.indexOf(keywords[i]);

        if (to < 0) {
            fields << QString();
            continue;
        } else {
            fields << m_fullDescription.mid(from, to - from).simplified();
            from = to + keywords[i].length();
        }
    }

    if (from < m_fullDescription.length()) {
        fields << m_fullDescription.mid(from).simplified();
    }

    m_bic = fields[1];
    m_sepa = fields[4];
    m_transactionId = fields[5];
    m_description = fields[6];
}

void Transaction::parseFullDescriptionIncasso()
{
    m_recurring = m_fullDescription.left(50).contains(QStringLiteral("doorlopend"));

    parseStandardFields();

    //special cases
    //OV chipkaart
    if (m_otherAccountNumber == QStringLiteral("NL12INGB0677515995")) {
        m_timeStamp = QDateTime::fromString(m_description.right(14),
                                            QStringLiteral("dd-MM-yy hh:mm"));
        if (std::abs(m_timeStamp.date().daysTo(m_date)) > 7) {
            m_timeStamp = m_timeStamp.addYears(100);
        }
        QRegularExpression re(QStringLiteral("35280\\d{11}"));
        QRegularExpressionMatch match = re.match(m_description);
        if (match.hasMatch()) {
            m_cardNumber = match.captured(0);
        }
    }
}

void Transaction::parseFullDescriptionPin()
{
    enum Fields{Card, Date, Time, ID};
    QStringList fields = m_fullDescription.split(QStringLiteral(" "),
                                                 QString::SkipEmptyParts);
    m_timeStamp = parseDateTime(fields[Date], fields[Time]);

    m_cardNumber = fields[Card].remove(QStringLiteral("Pasvolgnr:"),
                                       Qt::CaseInsensitive);
    m_transactionId = fields[ID].right(6);

    if (Settings::instance()->pinDateAsTransactionDate()) {
        m_date = m_timeStamp.date();
    }
}

void Transaction::parseFullDescriptionInternet()
{
    parseStandardFields();
    parseSavingsTransaction();
}

void Transaction::parseFullDescriptionCashWithdraw()
{
    if (m_fullDescription.contains(QStringLiteral("Pasvolgnr:"), Qt::CaseInsensitive)) {
        parseFullDescriptionPin();
    } else {
        enum Fields{Date, Time, Card};
        QStringList fields = m_fullDescription.split(QStringLiteral(" "), QString::SkipEmptyParts);
        m_timeStamp = parseDateTime(fields[Date], fields[Time]);

        m_cardNumber = fields[Card];
    }
    if (Settings::instance()->pinDateAsTransactionDate()) {
        m_date = m_timeStamp.date();
    }
}

QDateTime Transaction::parseDateTime(QString date, QString time) const
{
    QDate d = QDate::fromString(date, QStringLiteral("dd-MM-yyyy"));
    QTime t = QTime::fromString(time, QStringLiteral("hh:mm"));
    QDateTime dateTime(d, t);
    return dateTime;
}

void Transaction::parseSavingsTransaction()
{
    if (m_name.contains(QStringLiteral("Spaarrekening"),
                        Qt::CaseInsensitive) &&
        m_fullDescription.isEmpty())
    {
        Transaction* savingsTransaction = new Transaction(m_store);

        if (m_otherAccountNumber.isEmpty()) {
            m_otherAccountNumber = m_name.split(QStringLiteral(" ")).last();
        }
        if (m_type == TTTransfer) { //non-recurring would be of type TTInternet
            m_recurring = true;
        }
        m_type = m_type | TTSavingsFlag;

        savingsTransaction->m_mutationCents = -m_mutationCents;
        savingsTransaction->m_date = m_date;
        savingsTransaction->m_type = m_type;
        savingsTransaction->m_recurring = m_recurring;
        savingsTransaction->m_accountNumber = m_otherAccountNumber;
        savingsTransaction->m_otherAccountNumber = m_accountNumber;
        savingsTransaction->m_name = m_name;
        if (m_mutationCents > 0) {
            m_description = tr("Withdrawal from savings account %1").arg(otherAccountName());
            savingsTransaction->m_description = tr("Withdrawal to checking account %1").arg(accountName());
        } else {
            m_description = tr("Deposit into savings account %1").arg(otherAccountName());
            savingsTransaction->m_description = tr("Deposit from checking account %1").arg(accountName());
        }

        m_related.append(savingsTransaction);
        savingsTransaction->m_related.append(this);
    }
}

void Transaction::parseCreditCardTransaction()
{
    if (m_mutationCents < 0 &&
        m_type == TTOther &&
        m_fullDescription.startsWith(QStringLiteral(" INCASSO CREDITCARD ACCOUNTNR")) )
    {
        QRegularExpression re(QStringLiteral("\\d{10,}"));
        QRegularExpressionMatch match = re.match(m_fullDescription);
        if (match.hasMatch()) {
            m_otherAccountNumber = match.captured(0);
        }

        Transaction* creditCardTransaction = new Transaction(m_store);

        m_type = m_type | TTCreditCardFlag;

        creditCardTransaction->m_mutationCents = -m_mutationCents;
        creditCardTransaction->m_date = m_date;
        creditCardTransaction->m_type = m_type;
        creditCardTransaction->m_recurring = m_recurring;
        creditCardTransaction->m_accountNumber = m_otherAccountNumber;
        creditCardTransaction->m_otherAccountNumber = m_accountNumber;
        creditCardTransaction->m_name = m_name;
        if (m_mutationCents > 0) {
            m_description = tr("Withdrawal from credit card %1").arg(m_otherAccountNumber);
            creditCardTransaction->m_description = tr("Withdrawal to checking account %1").arg(m_accountNumber);
        } else {
            m_description = tr("Repay credit card %1").arg(m_otherAccountNumber);
            creditCardTransaction->m_description = tr("Repay credit card from %1").arg(m_accountNumber);
        }

        m_related.append(creditCardTransaction);
        creditCardTransaction->m_related.append(this);
    }
}

