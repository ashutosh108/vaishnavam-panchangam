#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "edit-custom-dates.h"
#include "html-table-writer.h"
#include "latlongedit.h"
#include "location.h"
#include "table-calendar-generator.h"
#include "text-interface.h"
#include "vrata_detail_printer.h"
#include "vrata-summary.h"
#include "tz-fixed.h"

#include <chrono>
#include "fmt-format-fixed.h"
#include <QDate>
#include <QMessageBox>
#include <QScrollBar>
#include <QShortcut>

void MainWindow::connectSignals()
{
    connect(ui->vrataSummary, &QTextBrowser::anchorClicked, [this]{
        expand_details_in_summary_tab = !expand_details_in_summary_tab;
        refreshAllTabs();
    });
    ui->tableTextBrowser->setOpenLinks(false);
    connect(ui->tableTextBrowser, &QTextBrowser::anchorClicked, [this](const QUrl & link){
        auto location_name = link.fragment();
        gui_ready = false;
        ui->locationComboBox->setCurrentText(location_name);
        ui->tabWidget->setCurrentIndex(0); // 0 means summary tab
        gui_ready = true;
        refreshAllTabs();
    });
}

void MainWindow::addTableContextMenu()
{
    addCustomDatesForTableAction = new QAction(tr("Add custom dates..."), this);
    connect(addCustomDatesForTableAction, &QAction::triggered, this, &MainWindow::addCustomDatesForTable);
    ui->tableTextBrowser->setContextMenuAction(addCustomDatesForTableAction);
}

void MainWindow::addCustomDatesForTable()
{
    custom_dates = vp::edit_custom_dates(custom_dates, this);
    refreshTable();
}

void MainWindow::setupToolbar()
{
    refraction_toggle = ui->toolBar->addAction("Refraction: on (click to toggle)", [this](bool checked){
        const auto text = QString{"Refraction: %1 (click to toggle)"}.arg(checked ? "on" : "off");
        refraction_toggle->setText(text);
        refreshAllTabs();
    });
    refraction_toggle->setCheckable(true);
    refraction_toggle->setChecked(true);
    refraction_toggle->setToolTip("on: take atmospheric refraction into account for sunrise/sunset; off: pure geometric sunrise/sunset, no refraction (can differ by 2-3 minutes)");

    ui->toolBar->addSeparator();

    sunrise_by_disc_center = ui->toolBar->addAction("Sunrise: by disc CENTER (click to toggle)", [this](bool checked) {
        const auto text = QString{"Sunrise: by disc %1 (click to toggle)"}.arg(checked ? "CENTER" : "EDGE");
        sunrise_by_disc_center->setText(text);
        refreshAllTabs();
    });
    sunrise_by_disc_center->setCheckable(true);
    sunrise_by_disc_center->setChecked(true);
    sunrise_by_disc_center->setToolTip("detect sunrise/sunset when sun disc's CENTER or EDGE crosses the horizon");

    ui->toolBar->addSeparator();

    ephemeris_swiss = ui->toolBar->addAction("Ephemeris: Swiss (click to change)", [this](bool checked) {
        const auto text = QString{"Ephemeris: %1 (click to change)"}.arg(checked ? "Swiss" : "Moshier");
        ephemeris_swiss->setText(text);
        refreshAllTabs();
    });
    ephemeris_swiss->setCheckable(true);
    ephemeris_swiss->setChecked(true);
    ephemeris_swiss->setToolTip("use Swiss ephemeris (better accuracy, requires se*.se1 files) or Moshier (less accurate, no data files required)");
}

vp::CalcFlags MainWindow::flagsForCurrentSettings()
{
    const auto refraction_flags = refraction_toggle->isChecked() ?
        vp::CalcFlags::RefractionOn
       :
       vp::CalcFlags::RefractionOff;
    const auto disc_flags = sunrise_by_disc_center->isChecked() ?
        vp::CalcFlags::SunriseByDiscCenter
        :
        vp::CalcFlags::SunriseByDiscEdge;
    const auto ephemeris_flags = ephemeris_swiss->isChecked() ?
        vp::CalcFlags::EphemerisSwiss
        :
        vp::CalcFlags::EphemerisMoshier;
    return refraction_flags | disc_flags | ephemeris_flags;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromStdString(vp::text_ui::program_name_and_version()));
    setupLocationsComboBox();
    setDateToToday();
    showVersionInStatusLine();
    setupKeyboardShortcuts();
    setupToolbar();
    connectSignals();
    addTableContextMenu();
    setupLocationInput();
    gui_ready = true;
    refreshAllTabs();
}

MainWindow::~MainWindow()
{
    delete ui;
}

date::sys_days to_sys_days(QDate qd)
{
    using namespace date;
    constexpr auto sys_epoch = sys_days{1970_y/January/1};
    constexpr auto jd_epoch = sys_days{year{-4713}/November/24};
    return sys_days{days{qd.toJulianDay()} - (sys_epoch - jd_epoch)};
}

// parse tiny subset of markdown in a single plain text line, return HTML version
QString htmlify_line(const std::string_view & line) {
    QString res = QString::fromUtf8(line.data(), line.size()).toHtmlEscaped();

    if (res.startsWith("# ")) {
        return "<h1>" + res + "</h1>";
    }

    res.replace(QRegularExpression{R"~((with Atiriktā (?:Ekādaśī|Dvādaśī)))~"}, R"(<span style="color:red">\1</span>)");

    // Highlight the second half or "on YYYY-MM-DD & YYYY-MM-DD" with red
    // Note: when changing this regex, be careful to not confuse "(" and ")"
    // of raw string literal syntax and of regexp itself.
    res.replace(QRegularExpression{R"~((on \d\d\d\d-\d\d-\d\d )(\S{1,10} \d\d\d\d-\d\d-\d\d))~"}, R"(\1<span style="color:red">\2</span>)");

    int pos = 0;
    while(true) {
        auto start = res.indexOf("**", pos);
        if (start == -1) break;
        auto end = res.indexOf("**", start+2);
        if (end == -1) break;
        constexpr int length_or_two_stars_and_b_and_b_close = std::char_traits<char>::length("**<b></b>"); // 9
        pos = end + length_or_two_stars_and_b_and_b_close;
        res = res.replace(start, 2, "<b>**");
        // end+3 because "<b>**" in previous replacement is three chars longer than "**"
        res = res.replace(end+3, 2, "**</b>");
    }
    return res;
}

static QString get_html_from_detail_view(const std::string_view & s) {
    QString res;
    for (auto first = s.data(), second = s.data(), last = first + s.size(); second != last && first != last; first = second + 1) {
        second = std::find(first, last, '\n');

        if (first != second) {
            res += htmlify_line(std::string_view{first, static_cast<std::string::size_type>(second - first)});
            res += "<br>\n";
        }
    }

    return res;
}

void MainWindow::setupLocationsComboBox()
{
    ui->locationComboBox->addItem("all");
    for (const auto &l : vp::text_ui::LocationDb()) {
        ui->locationComboBox->addItem(QString::fromUtf8(l.name.data(), l.name.size()));
    }
    ui->locationComboBox->setCurrentIndex(1); // select Udupi, first location after "all"
}

void MainWindow::setDateToToday()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

std::string MainWindow::selected_location() {
    if (ui->tableTextBrowser->isVisible()) {
        return "all";
    }
    return ui->locationComboBox->currentText().toStdString();
}

void MainWindow::recalcVratasForSelectedDateAndLocation() {
    auto date = to_sys_days(ui->dateEdit->date());
    auto location_string = selected_location();
    const auto flags = flagsForCurrentSettings();

    if (location_string == "all") {
        vratas = vp::text_ui::calc_all(date, flags);
    } else {
        vratas = vp::text_ui::calc_one(date, location_string, flags);
    }
}

void MainWindow::refreshAllTabs()
{
    if (!gui_ready) { return; }
    try {
        recalcVratasForSelectedDateAndLocation();
        refreshSummary();
        refreshTable();
        refreshDaybyday();
    } catch (std::exception &e) {
        QMessageBox::warning(this, "error", e.what());
    } catch (...) {
        QMessageBox::warning(this, "internal error", "unexpected exception thrown");
    }
}

static inline QString detailsLinkNonExpanded() {
    return R"(<div class="details"><a href="toggle-details:"><span class="monowidth">[+]</span> Details&nbsp;&rarr;</a></div>)";
}

static inline QString detailsLinkExpanded(const vp::Vrata & vrata) {
    QString detail_string = R"(<div class="details"><a href="toggle-details:"><span class="monowidth">[-]</span> Details</a><br>)";
    auto detail = vp::Vrata_Detail_Printer{vrata};
    detail_string += get_html_from_detail_view(fmt::format("{:c}", detail));
    detail_string += "</div>";
    return detail_string;
}

void MainWindow::refreshSummary()
{
    QString summary = R"CSS(
<style>p {font-size: 12pt;}
.date, .paran-range {font-size: 15pt;}
p.paran { margin-bottom: 0; } /* to ensure "Details->" link is not separated from the last line */
.monowidth { font-family: "Lucida Console", Monaco, monospace; }
</style>
    )CSS";

    bool first = true;

    for (const auto & vrata : vratas) {
        if (vrata.has_value()) {
            if (!first) {
                summary += "<p>&nbsp;</p>";
            }
            summary += QString::fromStdString(fmt::format("{}", vp::Vrata_Summary{&vrata.value()}));
            if (expand_details_in_summary_tab) {
                summary += detailsLinkExpanded(*vrata);
            } else {
                summary += detailsLinkNonExpanded();
            }
            first = false;
        }
    }

    int old_scroll_y = 0;
    if (QScrollBar * bar = ui->vrataSummary->verticalScrollBar(); bar) {
        old_scroll_y = bar->value();
    }
    ui->vrataSummary->setText(summary);
    if (QScrollBar * bar = ui->vrataSummary->verticalScrollBar(); bar) {
        bar->setValue(old_scroll_y);
    }
}

static QString table_css {R"CSS(<style>
table, td, th {
    border: 1px solid lightgray;
    font-family: Georgia, Serif;
}
td, th {
    padding: 0.1em;
}
td.mainpart, th.mainpart {
    text-align: center;
    vertical-align: middle;
}
th {
    background-color: #34CDF9;
    color: #FFFC9E;
}
th.mainpart {
    background-color: #c0c0c0;
    color: #3531ff;
}
tr.odd,td.odd {
    background-color: #EBF5FF;
}
td.odd.merge-to-top {
    border-top: 1px solid #EBF5FF;
}
td.odd.merge-to-bottom {
    border-bottom: 1px solid #EBF5FF;
}
tr.even,td.even {
    background-color: #D2E4FC;
}
td.even.merge-to-top {
    border-top: 1px solid #D2E4FC;
}
td.even.merge-to-bottom {
    border-bottom: 1px solid #D2E4FC;
}
td.vrata {
    background-color: #f8a102;
    font-weight: bold;
}
td.vrata.merge-to-bottom {
    border-bottom: 1px solid #f8a102;
}
td.vrata.merge-to-top {
    border-top: 1px solid #f8a102;
}
table {
    border-collapse: collapse;
}
tr.separator {
    height: 1.1em;
}
td.custom {
    background-color: yellow;
}
td.custom.merge-to-bottom {
    border-bottom: 1px solid yellow;
}
td.custom.merge-to-top {
    border-top: 1px solid yellow;
}
</style>)CSS"};

int MainWindow::getTableVerticalScrollValue() const
{
    const QScrollBar * scroll_bar = ui->tableTextBrowser->verticalScrollBar();
    return scroll_bar ? scroll_bar->value() : 0;
}

void MainWindow::setupLocationInput()
{
#ifdef VP_ARBITRARY_LOCATION_SELECTOR
    auto latlong_index = ui->locationVLayout->indexOf(ui->latLongHLayout);
    latlong_edit = new LatLongEdit({ui->latitude->getValue(), vp::Longitude{0.0}}, this);
    ui->locationVLayout->insertWidget(latlong_index+1, latlong_edit);
#endif
}

void MainWindow::refreshTable()
{
    if (!ui->tableTextBrowser->isVisible()) { return; }
    std::stringstream s;
    date::year current_year = date::year_month_day{date::floor<date::days>(std::chrono::system_clock::now())}.year();
    s << vp::Html_Table_Writer{vp::Table_Calendar_Generator::generate(vratas, current_year, custom_dates)};

    int old_scroll_y = getTableVerticalScrollValue();
    ui->tableTextBrowser->setHtmlForNormalAndSourceView(table_css + QString::fromStdString(s.str()));
    if (QScrollBar * bar = ui->tableTextBrowser->verticalScrollBar(); bar) {
        bar->setValue(old_scroll_y);
    }
}

namespace {
QString html_for_daybyday(const vp::text_ui::DayByDayInfo & info) {
    fmt::memory_buffer buf;
    fmt::format_to(buf,
                   "<h1>{} on {}</h1>\n",
                   info.location.name, info.date);
    auto tz = info.location.time_zone();
    if (info.saura_masa_until) {
        const auto saura_masa_until = date::make_zoned(tz, info.saura_masa_until->round_to_second());
        fmt::format_to(buf, FMT_STRING("Saura māsa: <big><b>{}</b></big> <small style=\"color:gray\">(until {})</small><br>\n"), info.saura_masa, date::format("%Y-%m-%d %H:%M:%S %Z", saura_masa_until));
    } else {
        fmt::format_to(buf, FMT_STRING("Saura māsa: <big><b>{}</b></big><br>\n"), info.saura_masa);
    }
    fmt::format_to(buf, FMT_STRING("Chāndra māsa: <big><b>{}</b></big>"), info.chandra_masa);
    if (info.chandra_masa_until) {
        const auto chandra_masa_until = date::make_zoned(tz, info.chandra_masa_until->round_to_second());
        fmt::format_to(buf, FMT_STRING(" <small style=\"color:gray\">(until {})</small>"), date::format("%Y-%m-%d %H:%M:%S %Z", chandra_masa_until));
    }
    fmt::format_to(buf, FMT_STRING("<br>\n"));
    if (info.sunrise1) {
        auto sunrise_date = date::floor<date::days>(date::make_zoned(tz, info.sunrise1->as_sys_time()).get_local_time());
        if (info.tithi_until) {
            const auto tithi_until = date::make_zoned(tz, info.tithi_until->round_to_minute());
            auto tithi_until_date{date::floor<date::days>(tithi_until.get_local_time())};
            fmt::format_to(buf,
                           FMT_STRING("<big><b>{}</b></big> until <big><b>{}{}</b></big><br>\n"),
                           info.tithi,
                           date::format("%H:%M", tithi_until),
                           tithi_until_date == sunrise_date ? "" : " next day");
        }
        if (info.tithi2_until) {
            const auto tithi2_until = date::make_zoned(tz, info.tithi2_until->round_to_minute());
            auto tithi2_until_date{date::floor<date::days>(tithi2_until.get_local_time())};
            fmt::format_to(buf,
                           FMT_STRING("<big><b>{}</b></big> until <big><b>{}{}</b></big><br>\n"),
                           info.tithi2,
                           date::format("%H:%M", tithi2_until),
                           tithi2_until_date == sunrise_date ? "" : " next day");
        }
        if (info.nakshatra_until) {
            const auto until = date::make_zoned(tz, info.nakshatra_until->round_to_minute());
            const auto date = date::floor<date::days>(until.get_local_time());
            fmt::format_to(buf,
                           FMT_STRING("<big><b>{}</b></big> until <big><b>{}{}</b></big><br>\n"),
                           info.nakshatra,
                           date::format("%H:%M", until),
                           date == sunrise_date ? "" : " next day");
        }
        if (info.nakshatra2_until) {
            const auto until = date::make_zoned(tz, info.nakshatra2_until->round_to_minute());
            const auto date = date::floor<date::days>(until.get_local_time());
            fmt::format_to(buf,
                           FMT_STRING("<big><b>{}</b></big> until <big><b>{}{}</b></big><br>\n"),
                           info.nakshatra2,
                           date::format("%H:%M", until),
                           date == sunrise_date ? "" : " next day");
        }
    }
    bool got_first_sunrise = false;
    bool got_second_sunrise = false;
    for (auto & e : info.events) {
        if (e.time_point == info.sunrise1) { got_first_sunrise = true; }
        if (e.time_point == info.sunrise2) { got_second_sunrise = true; }
        std::string color = (!got_first_sunrise || got_second_sunrise) ? "gray" : "";
        fmt::format_to(buf, FMT_STRING("<font color=\"{}\">"), color);
        fmt::format_to(buf, FMT_STRING("{} {}</font><br>\n"), vp::JulDays_Zoned{tz, e.time_point}, e.name);
    }
    return QString::fromStdString(fmt::to_string(buf));
}
}

void MainWindow::refreshDaybyday()
{
    if (!ui->daybydayBrowser->isVisible()) { return; }
    auto date = to_sys_days(ui->dateEdit->date());
    auto location_string = selected_location();
    const auto flags = flagsForCurrentSettings();
    auto location = vp::text_ui::LocationDb::find_coord(location_string.c_str());
    if (!location) {
        if (location_string != "all") {
            ui->daybydayBrowser->setPlainText(QString{"Can't find location: "} + QString::fromStdString(location_string));
        }
    } else {
        auto info = vp::text_ui::daybyday_calc_one(date, *location, flags);
        ui->daybydayBrowser->setHtml(html_for_daybyday(info));
    }
}

void MainWindow::showVersionInStatusLine()
{
    statusBar()->showMessage(QString::fromStdString(vp::text_ui::program_name_and_version()));
}

namespace {
// no need to store QShortcut objects as shortcut object's parent (MainWindow) will remove them properly
void new_shortcut(const QKeySequence &key, MainWindow *parent, void (MainWindow::*memberptr)()) {
    auto s = new QShortcut(key, parent);
    parent->connect(s, &QShortcut::activated, parent, memberptr);
}
template<class Lambda>
void new_shortcut(const QKeySequence &key, MainWindow *parent, Lambda lambda) {
    auto s = new QShortcut(key, parent);
    parent->connect(s, &QShortcut::activated, std::move(lambda));
}
}

void MainWindow::setupKeyboardShortcuts() {
    new_shortcut(Qt::CTRL + Qt::Key_PageUp, this, &MainWindow::switchToPrevTab);
    new_shortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab, this, &MainWindow::switchToPrevTab);
    new_shortcut(Qt::CTRL + Qt::Key_PageDown, this, &MainWindow::switchToNextTab);
    new_shortcut(Qt::CTRL + Qt::Key_Tab, this, &MainWindow::switchToNextTab);
    new_shortcut(Qt::CTRL + Qt::Key_Left, this, [this](){
        if (ui->tabWidget->tabText(ui->tabWidget->currentIndex()) == "Day-by-day") {
            ui->dateEdit->setDate(ui->dateEdit->date().addDays(-1));
        } else {
            on_datePrevEkadashi_clicked();
        }
    });
    new_shortcut(Qt::CTRL + Qt::Key_Right, this, [this]() {
        if (ui->tabWidget->tabText(ui->tabWidget->currentIndex()) == "Day-by-day") {
            ui->dateEdit->setDate(ui->dateEdit->date().addDays(+1));
        } else {
            on_dateNextEkadashi_clicked();
        }
    });
    new_shortcut(Qt::CTRL + Qt::Key_Home, this, &MainWindow::switchToHomeDateAndOrLocation);
    new_shortcut(Qt::CTRL + Qt::Key_Up, this, &MainWindow::switchToPrevLocation);
    new_shortcut(Qt::CTRL + Qt::Key_Down, this, &MainWindow::switchToNextLocation);
}

template <typename Widget>
inline void switchToPrev(Widget & widget) {
    int count = widget->count();
    if (count == 0) return;
    int new_index = (widget->currentIndex() - 1 + count) % count;
    widget->setCurrentIndex(new_index);
}

template <typename Widget>
inline void switchToNext(Widget & widget) {
    int count = widget->count();
    if (count == 0) return;
    int new_index = (widget->currentIndex() + 1) % count;
    widget->setCurrentIndex(new_index);
}

void MainWindow::switchToPrevTab()
{
    switchToPrev(ui->tabWidget);
}

void MainWindow::switchToNextTab()
{
    switchToNext(ui->tabWidget);
}

void MainWindow::switchToHomeDateAndOrLocation()
{
    // first ctrl+Home click: switch to today as starting date
    if (ui->dateEdit->date() != QDate::currentDate()) {
        return on_todayButton_clicked();
    }
    // second ctrl+Home click: switch to Udupi
    ui->locationComboBox->setCurrentIndex(1);
}

void MainWindow::switchToPrevLocation()
{
    switchToPrev(ui->locationComboBox);
}

void MainWindow::switchToNextLocation()
{
    switchToNext(ui->locationComboBox);
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
    if (location_name == "all") {
        clearLocationData();
    } else {
        auto location_arr = location_name.toUtf8();
        auto location = vp::text_ui::LocationDb().find_coord(location_arr.data());
        if (location.has_value()) {
            ui->latitude->setValue(location->latitude.latitude);
//            ui->latitude->setText(QString::fromStdString(fmt::to_string(location->latitude)));
            ui->longitude->setText(QString::fromStdString(fmt::to_string(location->longitude)));
            ui->timezone->setText(QString::fromStdString(location->time_zone()->name()));
#ifdef VP_ARBITRARY_LOCATION_SELECTOR
            if (latlong_edit) {
                latlong_edit->setCoord({location->latitude, location->longitude});
            }
#endif
        }
    }
    refreshAllTabs();
}

void MainWindow::on_tabWidget_currentChanged(int /*index*/)
{
    refreshAllTabs();
}

void MainWindow::on_dateEdit_dateChanged(const QDate & /*date*/)
{
    refreshAllTabs();
}

void MainWindow::on_todayButton_clicked()
{
    setDateToToday();
    refreshAllTabs();
}

static date::days to_juldays(date::sys_days date) {
    using namespace date;
    constexpr auto sys_epoch = sys_days{1970_y/January/1};
    constexpr auto jd_epoch = sys_days{year{-4713}/November/24};
    constexpr auto sys_to_jd = sys_epoch - jd_epoch;
    return date.time_since_epoch() + sys_to_jd;
}

static QDate to_qdate(date::sys_days date) {
    return QDate::fromJulianDay(to_juldays(date).count());
}

void MainWindow::on_dateNextEkadashi_clicked()
{
    auto next_date = vratas.guess_start_date_for_next_ekadashi(to_sys_days(ui->dateEdit->date()));
    ui->dateEdit->setDate(to_qdate(next_date));
}

void MainWindow::on_datePrevEkadashi_clicked()
{
    auto prev_date = vratas.guess_start_date_for_prev_ekadashi(to_sys_days(ui->dateEdit->date()));
    ui->dateEdit->setDate(to_qdate(prev_date));
}
