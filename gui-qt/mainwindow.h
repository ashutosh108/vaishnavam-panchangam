#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "table-calendar-generator.h"
#include "vrata.h"

#include "calc-flags.h"
#include "date-fixed.h"
#include <fmt/core.h>
#include <QAction>
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
    bool gui_ready = false; // set to true at the and of the MainWindow constructor
    bool expand_details_in_summary_tab = false;
    QAction * addCustomDatesForTableAction = nullptr;
    vp::Custom_Dates custom_dates;
    QAction * refraction_toggle = nullptr;
    QAction * sunrise_by_disc_center = nullptr;
    QAction * ephemeris_swiss = nullptr;

    void setupLocationsComboBox();
    void setDateToToday();
    void recalcVratasForSelectedDateAndLocation();
    void refreshAllTabs();
    void refreshSummary();
    void refreshTable();
    void showVersionInStatusLine();
    void clearLocationData();
    // "all" when table tab is active, otherwise whatever is selected in the locationComboBox
    std::string selected_location();
    void setupKeyboardShortcuts();
    void switchToPrevTab();
    void switchToNextTab();
    void switchToHomeDateAndOrLocation();
    void switchToPrevLocation();
    void switchToNextLocation();
    void connectSignals();
    void addTableContextMenu();
    void addCustomDatesForTable();
    void setupToolbar();
    vp::CalcFlags flagsForCurrentSettings();
};

QString htmlify_line(const std::string_view & line);

#endif // MAINWINDOW_H
