#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settings.h"
#include <QStyledItemDelegate>

#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

class RowDeleteDelegate: public QStyledItemDelegate
{
public:
    RowDeleteDelegate(QAbstractItemView* parent = 0);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    bool editorEvent(QEvent *event,
                     QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

private:
    QAbstractItemView* m_view;
    QImage m_deleteImage;
};

SettingsDialog::SettingsDialog(QAbstractItemModel *accountModel,
                               QAbstractItemModel *cardModel,
                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->tvAccountNames->setModel(accountModel);
    ui->tvCardNames->setModel(cardModel);

    loadSettings();

    ui->tvCardNames->setItemDelegate(new RowDeleteDelegate(ui->tvCardNames));
    ui->tvAccountNames->setItemDelegate(new RowDeleteDelegate(ui->tvAccountNames));

    connect(ui->cmdSelectPath, &QToolButton::clicked, this, &selectPath);
    connect(this, &QDialog::accepted, this, &saveSettings);
    connect(ui->cmdAddCard, &QPushButton::clicked, [cardModel]{
        cardModel->insertRow(cardModel->rowCount());
    });
    connect(ui->cmdAddAccount, &QPushButton::clicked, [accountModel]{
        accountModel->insertRow(accountModel->rowCount());
    });

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


RowDeleteDelegate::RowDeleteDelegate(QAbstractItemView *parent):
    QStyledItemDelegate(parent),
    m_view(parent),
    m_deleteImage(":/images/delete.png")
{
}

void RowDeleteDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    m_view->viewport()->setMouseTracking(true);
    m_view->viewport()->setAttribute(Qt::WA_Hover);
    QStyledItemDelegate::paint(painter, option, index);

    if (option.state.testFlag(QStyle::State_MouseOver)) {
        auto model = index.model();
        auto farRightIndex = model->index(index.row(),
                                          model->columnCount(index.parent()) -1,
                                          index.parent());
        auto rect = m_view->visualRect(farRightIndex);
        painter->save();
        painter->setClipRect(rect);
        painter->drawImage(rect.topRight()-QPoint(m_deleteImage.width(),
                                                  (m_deleteImage.height() - rect.height()) /2),
                           m_deleteImage);
        painter->restore();
    }
}

bool RowDeleteDelegate::editorEvent(QEvent *event,
                                    QAbstractItemModel *model,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index)
{
    if (event->type() == QEvent::Leave ||
        event->type() == QEvent::Enter)
    {
        m_view->viewport()->update();
    }

    if(index.column() != model->columnCount(index.parent()) -1) {
        return false;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto me = static_cast<QMouseEvent*>(event);
        auto rect = option.rect;
        rect.setLeft(rect.right() - m_deleteImage.width());
        if (rect.contains( me->localPos().toPoint()) ) {
            model->removeRow(index.row(), index.parent());
            return true;
        }
    }

    return false;
}
