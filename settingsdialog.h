#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}
class QAbstractItemModel;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QAbstractItemModel* accountModel,
                            QAbstractItemModel* cardModel,
                            QWidget *parent = 0);
    ~SettingsDialog();

private:
    void loadSettings();
    void saveSettings();
    void selectPath();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
