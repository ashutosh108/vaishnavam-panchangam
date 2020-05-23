#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iterator>
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

static QString htmlify_line(const std::string & line) {
    QString res = QString::fromStdString(line).toHtmlEscaped();

    if (res.startsWith("# ")) {
        return "<h1>" + res + "</h1>";
    }
    int pos = 0;
    while(1) {
        auto start = res.indexOf("**", pos);
        if (start == -1) break;
        auto end = res.indexOf("**", start+2);
        if (end == -1) break;
        // 9 is 2 (length of "**") + 3 (length of "<b>") + 4 (length of "</b>")
        pos = end + 9;
        res = res.replace(start, 2, "<b>**");
        // end+3 because "<b>**" in previous replacement is three chars longer than "**"
        res = res.replace(end+3, 2, "**</b>");
    }
    return res;
}

static QString get_html_from_detail_stream(std::stringstream & s) {
    QString res;
    s.seekg(0);
    for (std::string line; std::getline(s, line);) {
        res += htmlify_line(line);
        res += "<br>\n";
    }
    return res;
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

        QString detail_html = get_html_from_detail_stream(s);
        ui->calcResult->setHtml(detail_html);

//        ui->calcResult->setHtml("<h1>Detail</h1>" + Qt::convertFromPlainText(QString::fromStdString(s.str())));
//        ui->calcResult->setText(QString::fromStdString(s.str()));
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
        if (vp::is_atirikta(vrata->type)) {
            auto next_day = date::year_month_day{date::sys_days{vrata->date} + date::days{1}};
            vrata_date_s << " and " << next_day;
        }
        ui->vrataDate->setText(QString::fromStdString(vrata_date_s.str()));

        std::stringstream paran_next_day_s;
        auto paran_date = vrata->local_paran_date();
        paran_next_day_s << "Pāraṇam <span style=\" font-size:small;\">(" << paran_date << ")</span>:";
        ui->paranamNextDay->setText(QString::fromStdString(paran_next_day_s.str()));

        std::string paranTime = vp::ParanFormatter::format(
                    vrata->paran,
                    location->timezone_name,
                    "%H:%M<span style=\"font-size:small;\">:%S</span>",
                    "–",
                    "%H:%M<span style=\"font-size:small;\">:%S</span>",
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
    QMessageBox::about(this, "About", QString::fromStdString(vp::text_ui::program_name_and_version())
                       + "<br><br>Download new versions: <a href=\"https://github.com/ashutosh108/vaishnavam-panchangam/releases\">https://github.com/ashutosh108/vaishnavam-panchangam/releases</a>"
                       "<br><br>Support this program: <a href=\"https://www.patreon.com/vaishnavam_panchangam\">https://www.patreon.com/vaishnavam_panchangam</a>");
}

void MainWindow::on_actionE_xit_2_triggered()
{
    QApplication::quit();
}

void MainWindow::clearLocationData() {
    ui->latitude->setText("(multiple)");
    ui->longitude->setText("(multiple)");
    ui->timezone->setText("(multiple values)");
}

void MainWindow::on_locationComboBox_currentIndexChanged(const QString &location_name)
{
    if (location_name == "all") return clearLocationData();
    auto location_arr = location_name.toUtf8();
    auto location = vp::text_ui::LocationDb().find_coord(location_arr.data());
    if (!location.has_value()) return;
    ui->latitude->setText(QString::number(location->latitude));
    ui->longitude->setText(QString::number(location->longitude));
    ui->timezone->setText(location->timezone_name);
}
