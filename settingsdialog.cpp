#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    loadSettings();
    connect(ui->cmdSelectPath, &QToolButton::clicked, this, &selectPath);
    connect(this, &QDialog::accepted, this, &saveSettings);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings()
{
    Settings* settings = Settings::instance();
    ui->chkPinDateAsTransactionDate->setChecked(settings->pinDateAsTransactionDate());
    ui->chkSavingsDoubleBooking->setChecked(settings->doubleSavingsTransactions());
    ui->lblPathData->setText(settings->scanDirectory());
}

void SettingsDialog::saveSettings()
{
    Settings* settings = Settings::instance();
    settings->setDoubleSavingsTransactions(ui->chkSavingsDoubleBooking->isChecked());
    settings->setPinDateAsTransactionDate(ui->chkPinDateAsTransactionDate->isChecked());
    settings->setScanDirectory(ui->lblPathData->text());
}

void SettingsDialog::selectPath()
{
    auto fd = new QFileDialog(this);
    fd->setWindowTitle(tr("Select data directory"));
    fd->setDirectory(ui->lblPathData->text());
    fd->setFileMode(QFileDialog::Directory);
    fd->setOption(QFileDialog::ShowDirsOnly, true);
    connect(fd, &QFileDialog::fileSelected,
            [=](QString fileName)
            {
                if (!fileName.isEmpty()) {
                    ui->lblPathData->setText(fileName);
                }
            });
    connect(fd, &QFileDialog::finished, fd, &QFileDialog::deleteLater);

    fd->open();
}
