#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "text-interface.h"
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
    void make_all_qmessagebox_texts_selectable() {
        setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");
    }
};

int main(int argc, char *argv[])
{
    vp::text_ui::change_to_data_dir(argv[0]);
    MyApplication a(argc, argv);
    a.make_all_qmessagebox_texts_selectable();
    date::set_install("tzdata");
    MainWindow w;
    w.show();
    return a.exec();
}
