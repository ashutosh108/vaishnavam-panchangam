#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "date-fixed.h"
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

private:
    Ui::MainWindow *ui;
    void setupLocationsComboBox();
    void setDateToToday();
    void calcAll(date::year_month_day base_date, std::ostream & o);
    void calcOne(date::year_month_day base_date, QString location, std::ostream & o);
};
#endif // MAINWINDOW_H
