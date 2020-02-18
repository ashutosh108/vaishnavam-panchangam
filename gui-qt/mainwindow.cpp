#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDate>
#include <QMessageBox>
#include <sstream>

#include "calc.h"
#include "juldays_ut.h"
#include "vrata_detail.h"
#include "location.h"
#include "text-interface.h"
#include "tz-fixed.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromStdString(vp::text_ui::program_name_and_version()));
    setupLocationsComboBox();
    setDateToToday();
    showVersionInStatusLine();
}

MainWindow::~MainWindow()
{
    delete ui;
}

date::sys_days to_sys_days(QDate qd)
{
    using namespace date;
    return sys_days{days{qd.toJulianDay()} -
        (sys_days{1970_y/January/1} - sys_days{year{-4713}/November/24})};
}

void MainWindow::on_FindNextEkadashi_clicked()
{
    try {
        date::year_month_day date = to_sys_days(ui->dateEdit->date());

        auto location_string = ui->locationComboBox->currentText();

        std::stringstream s;
        if (location_string == "all") {
            calcAll(date, s);
        } else {
            calcOne(date, location_string, s);
        }

        ui->calcResult->setText(QString::fromStdString(s.str()));
    } catch (std::exception &e) {
        QMessageBox::warning(this, "error", e.what());
    } catch (...) {
        QMessageBox::warning(this, "internal error", "unexpected exception thrown");
    }
}

void MainWindow::setupLocationsComboBox()
{
    ui->locationComboBox->addItem("all");
    for (const auto &l : vp::text_ui::LocationDb()) {
        ui->locationComboBox->addItem(l.name);
    }
}

void MainWindow::setDateToToday()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

void MainWindow::calcAll(date::year_month_day base_date, std::ostream &o)
{
    for (auto &l : vp::text_ui::LocationDb()) {
        calcOne(base_date, l.name, o);
    }
//    vp::text_ui::calc_all(base_date, o);
}

void MainWindow::calcOne(date::year_month_day base_date, QString location_string, std::ostream &o)
{
    QByteArray location_as_bytearray = location_string.toLocal8Bit();
    char * location_name = location_as_bytearray.data();

    std::optional<vp::Location> location = vp::text_ui::LocationDb::find_coord(location_name);
    if (!location) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }

    auto vrata = vp::Calc{*location}.find_next_vrata(base_date);
    if (!vrata.has_value()) {
        o << location_name << ": calculation error, can't find next Ekadashi. Sorry.\n";
    } else {
        ui->locationName->setText(location_string);

        std::stringstream vrata_type_s;
        vrata_type_s << vrata->type;
        ui->vrataType->setText(QString::fromStdString(vrata_type_s.str()));

        std::stringstream vrata_date_s;
        vrata_date_s << vrata->date;
        ui->vrataDate->setText(QString::fromStdString(vrata_date_s.str()));

        std::string paranTime = vp::ParanFormatter::format(
                    vrata->paran,
                    location->timezone_name,
                    "%H:%M<span style=\"color:darkgray; font-size:small;\">:%S</span>",
                    "–",
                    "%H:%M<span style=\"color:darkgray; font-size:small;\">:%S</span>",
                    "<sup>*</sup><br><small><sup>*</sup>");
        paranTime += "</small>";
        ui->paranTime->setText(QString::fromStdString(paranTime));

        vp::Vrata_Detail vd{*vrata, *location};
        o << vd << "\n\n";
    }
}

void MainWindow::showVersionInStatusLine()
{
    statusBar()->showMessage(QString::fromStdString(vp::text_ui::program_name_and_version()));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About", QString::fromStdString(vp::text_ui::program_name_and_version()));
}

void MainWindow::on_actionE_xit_2_triggered()
{
    QApplication::quit();
}
