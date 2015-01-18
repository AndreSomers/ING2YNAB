#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transactionstore.h"
#include "transaction.h"
#include "csv.h"
#include "filterengine.h"
#include "settings.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QDirIterator>
#include <QLabel>
#include "transactionmodelfilter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_store(new TransactionStore(this)),
    m_filters(new FilterEngine(this))
{
    ui->setupUi(this);
    m_lblFileCount = new QLabel(this);
    m_lblRecordCount = new QLabel(this);
    m_lblAccountCount = new QLabel(this);
    m_lblDateRange = new QLabel(this);
    m_lblTotalIn = new QLabel(this);
    m_lblTotalOut = new QLabel(this);
    m_lblTotalBalance = new QLabel(this);
    m_lblTotalSavings = new QLabel(this);
    ui->statusBar->addWidget(m_lblFileCount);
    ui->statusBar->addWidget(m_lblRecordCount);
    ui->statusBar->addWidget(m_lblAccountCount);
    ui->statusBar->addWidget(m_lblDateRange);
    ui->statusBar->addWidget(m_lblTotalIn);
    ui->statusBar->addWidget(m_lblTotalOut);
    ui->statusBar->addWidget(m_lblTotalBalance);
    ui->statusBar->addWidget(m_lblTotalSavings);

    auto filter = new TransactionModelFilter(this);
    filter->setSourceModel(m_store->model());
    ui->transactionView->setModel(filter);

    filter->sort(0, Qt::AscendingOrder);

    connect(ui->cmdCreateYNABExport, &QCommandLinkButton::clicked,
            this, &MainWindow::createYNABExport);

    connect(m_store, SIGNAL(transactionAdded(Transaction*)),
            m_filters, SLOT(filter(Transaction*)));

    connect(ui->leFilter, SIGNAL(textChanged(QString)),
            filter, SLOT(setFilterString(QString)));

    loadSettings();

    scanForIngFiles();
    updateSummaryValues();
}

MainWindow::~MainWindow()
{
    if (Settings::instance()->saveSettingsOnExit()) {
        saveSettings();
    }

    delete ui;
}

const auto litIngFileHeader = QStringLiteral("\"Datum\",\"Naam / Omschrijving\",\"Rekening\",\"Tegenrekening\",\"Code\",\"Af Bij\",\"Bedrag (EUR)\",\"MutatieSoort\",\"Mededelingen\"");

void MainWindow::scanForIngFiles()
{
    QDirIterator it(Settings::instance()->scanDirectory(),
                    QStringList{QStringLiteral("*.csv")},
                    QDir::Files | QDir::NoDotAndDotDot);
    while(it.hasNext()) {
        it.next();
        QFile file(it.filePath());
        if (file.open(QFile::ReadOnly)) {
            QString firstLine = QString::fromLocal8Bit(file.readLine().trimmed());
            file.close();
            if (firstLine == litIngFileHeader) {
                //ok, this looks like an ING file
                m_store->addFromFile(it.filePath());
            }
        }
    }
}

void MainWindow::createYNABExport()
{
    QHash<QString, QList<QStringList> > output;

    for(int i(0); i < m_store->count(); ++i) {
        Transaction* trans = m_store->transaction(i);

        if (!output.contains(trans->accountName())) {
            QList<QStringList> accountOutput;
            const QStringList header{
                QStringLiteral("Date"),
                QStringLiteral("Payee"),
                QStringLiteral("Category"),
                QStringLiteral("Memo"),
                QStringLiteral("Outflow"),
                QStringLiteral("Inflow")};

            accountOutput << header;
            output.insert(trans->accountName(), accountOutput);
        }
        QList<QStringList>& accountOutput = output[trans->accountName()];

        QStringList row;
        row << trans->date().toString("dd/MM/yyyy");
        row << trans->name();
        row << trans->category();
        row << trans->description();
        row << trans->mutationOut();
        row << trans->mutationIn();
        accountOutput << row;
    }

    //save an output file for each account
    QHashIterator<QString, QList<QStringList> > it(output);
    while(it.hasNext()) {
        it.next();
        QString fileName= QString("C:/Users/Andre/Downloads/ynab import %1 %2-%3.csv")
                .arg(it.key())
                .arg(m_store->fromDate().toString("yyyyMMdd"))
                .arg(m_store->toDate().toString("yyyyMMdd"));
        CSV::write(it.value(), fileName);
    }

}

void MainWindow::updateSummaryValues()
{
    QLocale locale;
    m_lblFileCount->setText(tr("%n files", "", m_store->fileCount()));
    m_lblRecordCount->setText(tr("%n records", "", m_store->count()));
    m_lblAccountCount->setText(tr("%n accounts", "", m_store->accountCount()));
    m_lblDateRange->setText(tr("from %1 to %2")
                         .arg(m_store->fromDate().toString(Qt::DefaultLocaleShortDate))
                         .arg(m_store->toDate().toString(Qt::DefaultLocaleShortDate)));

    m_lblTotalIn->setText(tr("total in: %1")
                          .arg(locale.toCurrencyString(m_store->totalIn()/100.0)));
    m_lblTotalOut->setText(tr("total out: %1")
                           .arg(locale.toCurrencyString(m_store->totalOut()/100.0)));
    quint64 balance = m_store->totalIn() - m_store->totalOut();
    m_lblTotalBalance->setText(tr("transfer balance: %1")
                               .arg(locale.toCurrencyString(balance/100.0)));
    m_lblTotalSavings->setText(tr("savings transfers: %1")
                               .arg(locale.toCurrencyString(m_store->totalSavings()/100.0)));
}

void MainWindow::loadSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QString settingsPath = QDir(path).filePath(QStringLiteral("settings.json"));

    qDebug() << "Attempting to load settings from" << settingsPath;

    if(path.isEmpty() || !QFileInfo(settingsPath).exists())
        return;

    QFile settingsFile(settingsPath);
    if (settingsFile.open(QFile::ReadOnly)) {
        QByteArray data = settingsFile.readAll();
        QJsonParseError error;
        QJsonDocument settings = QJsonDocument::fromJson(data, &error);
        if (error.error == QJsonParseError::NoError) {
            bool ok(true);
            QJsonObject obj = settings.object();
            ok &= m_filters->load(obj["filters"]);
            StringHash cardNames, accountNames;
            ok &= cardNames.load(obj["cardNames"]);
            ok &= accountNames.load(obj["accountNames"]);
            m_store->setAccountNameMap(accountNames);
            m_store->setCardNameMap(cardNames);
            ok &= Settings::instance()->load(obj["settings"]);
            Settings::instance()->setSaveSettingsOnExit(ok);
        } else {
            qWarning() << QString(QStringLiteral("Error parsing settings file %1: %2"))
                          .arg(settingsPath, error.errorString());
            Settings::instance()->setSaveSettingsOnExit(false);
        }
        settingsFile.close();
    } else {
        Settings::instance()->setSaveSettingsOnExit(true);
    }
}

void MainWindow::saveSettings()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if(path.isEmpty())
        return;

    QString settingsPath = QDir(path).filePath(QStringLiteral("settings.json"));

    QDir(settingsPath).mkpath(path);
    QFile settingsFile(settingsPath);
    if (settingsFile.open(QFile::WriteOnly)) {
        QJsonObject settingsObject;
        settingsObject["filters"] = m_filters->save();
        settingsObject["cardNames"] = m_store->cardNameMap().save();
        settingsObject["accountNames"] = m_store->accountNameMap().save();
        settingsObject["settings"] = Settings::instance()->save();
        QJsonDocument settingsDocument(settingsObject);
        settingsFile.write(settingsDocument.toJson());
        settingsFile.close();
    } else {
        qWarning() << QString(QStringLiteral("Could not open settings file '%1' for writing!"))
                      .arg(settingsPath);
    }
}
