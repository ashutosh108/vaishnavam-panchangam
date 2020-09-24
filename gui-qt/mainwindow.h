#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "vrata.h"

#include "date-fixed.h"
#include <fmt/core.h>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_FindNextEkadashi_clicked();

    void on_actionAbout_triggered();

    void on_actionE_xit_2_triggered();

    void on_locationComboBox_currentIndexChanged(const QString &arg1);

    void on_tabWidget_currentChanged(int index);

    void on_dateEdit_dateChanged(const QDate &date);

    void on_todayButton_clicked();

    void on_dateNextEkadashi_clicked();

    void on_datePrevEkadashi_clicked();

private:
    Ui::MainWindow *ui;
    vp::VratasForDate vratas;
    void setupLocationsComboBox();
    void setDateToToday();
    void calcAll(date::year_month_day base_date, fmt::memory_buffer & buf);
    void calcOne(date::year_month_day base_date, QString location, fmt::memory_buffer & buf);
    void refreshAllTabs();
    void refreshSummary();
    void refreshTable();
    void showVersionInStatusLine();
    void clearLocationData();
};

QString htmlify_line(const std::string_view & line);

#endif // MAINWINDOW_H
