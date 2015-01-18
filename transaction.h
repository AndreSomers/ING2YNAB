#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QObject>
#include <QDate>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QLocale>

class TransactionStore;

class Transaction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int type READ type)
    Q_PROPERTY(QString typeName READ typeName)
    Q_PROPERTY(QDate date READ date)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString accountNumber READ accountNumber)
    Q_PROPERTY(QString accountName READ accountName)
    Q_PROPERTY(QString otherAccountNumber READ otherAccountNumber)
    Q_PROPERTY(QString otherAccountName READ otherAccountName)
    Q_PROPERTY(qint64 mutation READ mutationCents)
    Q_PROPERTY(QString mutationIn READ mutationIn)
    Q_PROPERTY(QString mutationOut READ mutationOut)
    Q_PROPERTY(QString fullDescription READ fullDescription)
    Q_PROPERTY(QString cardNumber READ cardNumber)
    Q_PROPERTY(QString cardName READ cardName)
    Q_PROPERTY(QDateTime timeStamp READ timeStamp)
    Q_PROPERTY(bool recurring READ recurring)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString sepa READ sepa)
    Q_PROPERTY(QString bic READ bic)
    Q_PROPERTY(QString transactionId READ transactionId)
    Q_PROPERTY(QString category MEMBER m_category NOTIFY categoryChanged)

public:
    enum TransactionType {
        TTTransfer = 1,
        TTPinTransaction = 2,
        TTCashWithdrawal = 3,
        TTCollection = 4,
        TTInternet = 5,
        TTMassTransfer = 6,
        TTOther = 0,
        TTSavingsFlag = 64,
        TTCreditCardFlag = 128,
    };

    static QString nameForType(int type);

public:
    explicit Transaction(QStringList rawRecord, QString sourceFile, TransactionStore *parent = 0);
    ~Transaction();

    int type() const {return m_type;}
    QString typeName() const;
    QDate date() const {return m_date;}
    QString name() const {return m_name;}
    QString accountNumber() const {return m_accountNumber;}
    QString accountName() const;
    QString otherAccountNumber() const {return m_otherAccountNumber;}
    QString otherAccountName() const;
    qint64 mutationCents() const {return m_mutationCents;}
    QString mutationIn() const {return m_mutationCents < 0 ? QString() : QLocale().toCurrencyString( m_mutationCents/100.0, QString(" ") );}
    QString mutationOut() const {return m_mutationCents < 0 ? QLocale().toCurrencyString( -m_mutationCents/100.0, QString(" ") ) : QString();}
    QString fullDescription() const {return m_fullDescription;}
    QString cardNumber() const {return m_cardNumber;}
    QString cardName() const;
    QDateTime timeStamp() const {return m_timeStamp;}
    bool recurring() const {return m_recurring;}
    QString description() const {return m_description;}
    QString sepa() const {return m_sepa;}
    QString bic() const {return m_bic;}
    QString transactionId() const {return m_transactionId;}
    QString category() const {return m_category;}
    QString sourceFile() const {return m_sourceFile;}

    QList<Transaction*> relatedTransactions() const {return m_related;}
    TransactionStore* store() const {return m_store;}

public slots:
    void setDescription(QString description);

signals:
    void descriptionChanged(QString description);
    void categoryChanged(QString category);
    void cardNameChanged(QString cardName);

private: //methods
    Transaction(TransactionStore *parent);

    void parseRawRecord(QStringList rawRecord);
    void parseStandardFields();
    void parseFullDescriptionTransfer();
    void parseFullDescriptionIncasso();
    void parseFullDescriptionPin();
    void parseFullDescriptionInternet();
    void parseFullDescriptionCashWithdraw();
    QDateTime parseDateTime(QString date, QString time) const;
    void parseSavingsTransaction();
    void parseCreditCardTransaction();

private: //members
    TransactionStore* m_store;

    int m_type;
    QDate m_date;
    QString m_name;
    QString m_accountNumber;
    QString m_otherAccountNumber;
    qint64 m_mutationCents;
    QString m_fullDescription;
    QString m_cardNumber;
    QDateTime m_timeStamp;
    bool m_recurring;
    QString m_description;
    QString m_sepa;
    QString m_transactionId;
    QString m_bic;
    QString m_category;
    QString m_sourceFile;

    QList<Transaction*> m_related;
};

#endif // TRANSACTION_H
