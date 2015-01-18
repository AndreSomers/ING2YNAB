#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QLabel;

namespace Ui {
class MainWindow;
}
class TransactionStore;
class FilterEngine;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void scanForIngFiles();


private slots:
    void createYNABExport();
    void updateSummaryValues();

    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;
    TransactionStore* m_store;
    FilterEngine* m_filters;

    QLabel* m_lblFileCount;
    QLabel* m_lblRecordCount;
    QLabel* m_lblAccountCount;
    QLabel* m_lblDateRange;
    QLabel* m_lblTotalIn;
    QLabel* m_lblTotalOut;
    QLabel* m_lblTotalBalance;
    QLabel* m_lblTotalSavings;
};

#endif // MAINWINDOW_H
