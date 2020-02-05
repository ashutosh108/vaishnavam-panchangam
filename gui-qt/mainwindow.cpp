#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    setupLocationsComboBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_FindNextEkadashi_clicked()
{
    try {
        auto date_string = ui->dateLineEdit->text();
        QByteArray date_as_bytearray = date_string.toLocal8Bit();
        auto base_date = vp::text_ui::parse_ymd(date_as_bytearray.data());

        auto location_string = ui->locationComboBox->currentText();

        std::stringstream s;
        if (location_string == "all") {
            calcAll(base_date, s);
        } else {
            calcOne(base_date, location_string, s);
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

    std::optional<vp::Location> coord = vp::text_ui::LocationDb::find_coord(location_name);
    if (!coord) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }

    auto vrata = vp::Calc{*coord}.find_next_vrata(base_date);
    if (!vrata.has_value()) {
        o << location_name << ": calculation error, can't find next Ekadashi. Sorry.\n";
    } else {
        ui->locationName->setText(location_name);

        std::stringstream vrata_type_s;
        vrata_type_s << vrata->type;
        ui->vrataType->setText(QString::fromStdString(vrata_type_s.str()));

        std::stringstream vrata_date_s;
        vrata_date_s << vrata->date;
        ui->vrataDate->setText(QString::fromStdString(vrata_date_s.str()));

        auto timezone = date::locate_zone(coord->timezone_name);
        std::stringstream paran_s;
        if (vrata->paran.paran_start.has_value()) {
            auto rounded_up = date::ceil<std::chrono::seconds>(vrata->paran.paran_start->as_sys_time());
            auto zoned = date::make_zoned(timezone, rounded_up);
            paran_s << date::format("%H:%M<span style=\"color:darkgray; font-size:small;\">:%S</span>", zoned);
        } else {
            paran_s << '?';
        }
        paran_s << "–";
        if (vrata->paran.paran_end.has_value()) {
            auto rounded_down = date::floor<std::chrono::seconds>(vrata->paran.paran_end->as_sys_time());
            auto zoned = date::make_zoned(timezone, rounded_down);
            paran_s << date::format("%H:%M<span style=\"color:darkgray; font-size:small;\">:%S</span>", zoned);
        } else {
            paran_s << '?';
        }
        ui->paranTime->setText(QString::fromStdString(paran_s.str()));

        vp::Vrata_Detail vd{*vrata, *coord};
        o << location_name << '\n' << vd << "\n\n";
    }
}
