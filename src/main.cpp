#include "gui/mainwindow.hpp"
#include "base/settings.hpp"
#include "utils/defs.hpp"

#include <QApplication>


int main (int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setCursorFlashTime(App::CursorBlinkInterval);

    QApplication::setOrganizationDomain(App::OrgName);
    QApplication::setApplicationName(App::AppName);

    Settings::loadSettings();

    app.setStyle(App::Style);

    MainWindow main_window;

    // Hack for my wm to have this window floating
    // without configuring a rule for it in the wm
    main_window.setWindowFlags(Qt::Dialog);
    main_window.show();

    int app_return_status = app.exec();

    if (app_return_status == 0) {
        Settings::saveSettings();
    }

    return app_return_status;
}

