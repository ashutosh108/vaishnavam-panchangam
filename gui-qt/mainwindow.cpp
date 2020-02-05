#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <sstream>

#include "text-interface.h"

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
            vp::text_ui::calc_all(base_date, s);
        } else {
            QByteArray location_as_bytearray = location_string.toLocal8Bit();
            char * location_as_cstr = location_as_bytearray.data();

            vp::text_ui::calc_one(
                        base_date, location_as_cstr, s);
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
