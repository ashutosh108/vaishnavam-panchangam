#include <filesystem>
#include <iostream>
#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "tz-fixed.h"
#include "mainwindow.h"


class MyApplication : public QApplication {
public:
    MyApplication(int& argc, char** argv) :
      QApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event) override {
        bool done = true;
        try {
            done = QApplication::notify(receiver, event);
        } catch (const std::exception &e) {
            QMessageBox::warning(nullptr, "fatal error", e.what());
        }
        return done;
    }
};

int main(int argc, char *argv[])
{
    MyApplication a(argc, argv);
    auto curdir = QCoreApplication::applicationDirPath();
    QDir::setCurrent(curdir + "/..");
    std::cerr << "current_path: " << std::filesystem::current_path() << '\n';
    date::set_install("tzdata");
    MainWindow w;
    w.show();
    return a.exec();
}
