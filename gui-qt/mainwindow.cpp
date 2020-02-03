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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_FindNextEkadashi_clicked()
{
    auto date_string = ui->dateLineEdit->text();
    QByteArray date_as_bytearray = date_string.toLocal8Bit();
    auto base_date = vp::text_ui::parse_ymd(date_as_bytearray.data());
    auto location_string = ui->LocationLineEdit->text();
    QByteArray location_as_bytearray = location_string.toLocal8Bit();
    char * location_as_cstr = location_as_bytearray.data();

    std::stringstream s;
    vp::text_ui::calc_one(
                base_date, location_as_cstr, s);

    ui->calcResult->setText(QString::fromStdString(s.str()));
}
