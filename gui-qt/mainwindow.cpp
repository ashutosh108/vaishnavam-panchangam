#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "edit-custom-dates.h"
#include "html-table-writer.h"
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
        auto text = QString{"Refraction: %1 (click to toggle)"}.arg(checked ? "on" : "off");
        refraction_toggle->setText(text);
        refreshAllTabs();
    });
    refraction_toggle->setCheckable(true);
    refraction_toggle->setChecked(true);
    refraction_toggle->setToolTip("on: take atmospheric refraction into account for sunrise/sunset; off: pure geometric sunrise/sunset, no refraction (can differ by 2-3 minutes)");
}

vp::CalcFlags MainWindow::flagsForCurrentSettings()
{
    if (refraction_toggle->isChecked()) {
        return vp::CalcFlags::RefractionOn;
    } else {
        return vp::CalcFlags::RefractionOff;
    }
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
    ui->vrataSummary->setText(summary);
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

void MainWindow::refreshTable()
{
    if (!ui->tableTextBrowser->isVisible()) { return; }
    std::stringstream s;
    date::year current_year = date::year_month_day{date::floor<date::days>(std::chrono::system_clock::now())}.year();
    s << vp::Html_Table_Writer{vp::Table_Calendar_Generator::generate(vratas, current_year, custom_dates)};
    ui->tableTextBrowser->setHtmlForNormalAndSourceView(table_css + QString::fromStdString(s.str()));
}

void MainWindow::showVersionInStatusLine()
{
    statusBar()->showMessage(QString::fromStdString(vp::text_ui::program_name_and_version()));
}

void MainWindow::setupKeyboardShortcuts() {
    // no need to store as shortcut object's parent (MainWindow) will remove it properly
    new QShortcut(Qt::CTRL + Qt::Key_PageUp, this, this, &MainWindow::switchToPrevTab);
    new QShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab, this, this, &MainWindow::switchToPrevTab);
    new QShortcut(Qt::CTRL + Qt::Key_PageDown, this, this, &MainWindow::switchToNextTab);
    new QShortcut(Qt::CTRL + Qt::Key_Tab, this, this, &MainWindow::switchToNextTab);
    new QShortcut(Qt::CTRL + Qt::Key_Left, this, this, &MainWindow::on_datePrevEkadashi_clicked);
    new QShortcut(Qt::CTRL + Qt::Key_Right, this, this, &MainWindow::on_dateNextEkadashi_clicked);
    new QShortcut(Qt::CTRL + Qt::Key_Home, this, this, &MainWindow::switchToHomeDateAndOrLocation);
    new QShortcut(Qt::CTRL + Qt::Key_Up, this, this, &MainWindow::switchToPrevLocation);
    new QShortcut(Qt::CTRL + Qt::Key_Down, this, this, &MainWindow::switchToNextLocation);
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
