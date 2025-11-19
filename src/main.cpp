#include "gui/mainwindow.hpp"

#include "utils/defs.hpp"

#include <QApplication>


int main (int argc, char **argv) {
    QApplication app(argc, argv);

    QApplication::setOrganizationDomain(App::AppName);
    QApplication::setApplicationName(App::AppName);

    app.setStyle(App::Style);

    MainWindow main_window;

    // Hack for my wm to have this window floating
    // without configuring a rule for it
    main_window.setWindowFlags(Qt::Dialog);
    main_window.show();

    return app.exec();
}
