#include "base/settings.hpp"

#include <QDir>


QSettings Settings::settings = QSettings(App::OrgName, App::AppName);

bool Settings::scale_image = true;
int Settings::scale_slider_value = 0;
float Settings::image_scale_value = 1.0;

ImageOptions::ImageOptions Settings::imageview_option = ImageOptions::FitInView;

bool Settings::search_while_typing = false;
bool Settings::select_first_item = false;

bool Settings::remember_settings = false;

bool Settings::load_last_database = false;
QString Settings::last_database_path = QString();

QString Settings::last_add_file_dialog_path = QDir::homePath();
QString Settings::last_add_dir_dialog_path= QDir::homePath();
QString Settings::last_select_database_dialog_path = QDir::homePath();



void Settings::saveSettings() {
    Settings::settings.setValue(QStringLiteral("scale_image"), Settings::scale_image);
    Settings::settings.setValue(QStringLiteral("scale_slider_value"), Settings::scale_slider_value);

    Settings::settings.setValue(QStringLiteral("search_while_typing"), Settings::search_while_typing);
    Settings::settings.setValue(QStringLiteral("select_first_item"), Settings::select_first_item);

    Settings::settings.setValue(QStringLiteral("remember_settings"), Settings::remember_settings);

    Settings::settings.setValue(QStringLiteral("load_last_database"), Settings::load_last_database);
    Settings::settings.setValue(QStringLiteral("last_database_path"), Settings::last_database_path);

    Settings::settings.setValue(QStringLiteral("last_add_file_dialog_path"), Settings::last_add_file_dialog_path);
    Settings::settings.setValue(QStringLiteral("last_add_dir_dialog_path"), Settings::last_add_dir_dialog_path);
    Settings::settings.setValue(QStringLiteral("last_select_database_dialog_path"), Settings::last_select_database_dialog_path);

}

void Settings::loadSettings() {
    Settings::scale_image = Settings::settings.value(QStringLiteral("scale_image"), true).toBool();
    Settings::scale_slider_value = Settings::settings.value(QStringLiteral("scale_slider_value"), 0).toInt();

    Settings::imageview_option = ImageOptions::FitInView;

    Settings::search_while_typing = Settings::settings.value(QStringLiteral("search_while_typing"), false).toBool();
    Settings::select_first_item = Settings::settings.value(QStringLiteral("select_first_item"), false).toBool();

    Settings::remember_settings = Settings::settings.value(QStringLiteral("remember_settings"), false).toBool();

    Settings::load_last_database = Settings::settings.value(QStringLiteral("load_last_database"), false).toBool();
    Settings::last_database_path = Settings::settings.value(QStringLiteral("last_database_path"), QString()).toString();

    Settings::last_add_file_dialog_path = Settings::settings.value(QStringLiteral("last_add_file_dialog_path"), QDir::homePath()).toString();
    Settings::last_add_dir_dialog_path = Settings::settings.value(QStringLiteral("last_add_dir_dialog_path"), QDir::homePath()).toString();
    Settings::last_select_database_dialog_path = Settings::settings.value(QStringLiteral("last_select_database_dialog_path"), QDir::homePath()).toString();


    if (Settings::remember_settings == false) {
        Settings::loadDefaultSettings();
    } else {
        Settings::updateImageScaleValue();
    }

}

void Settings::loadDefaultSettings() {
    Settings::scale_image = true;
    Settings::scale_slider_value = 0;
    Settings::image_scale_value = 1.0;

    Settings::imageview_option = ImageOptions::FitInView;

    Settings::search_while_typing = false;
    Settings::select_first_item = false;

    Settings::remember_settings = false;

    Settings::load_last_database = false;

    Settings::last_database_path = QString();
}

void Settings::updateImageScaleValue() {
    Settings::image_scale_value = 1.0 + (Settings::scale_slider_value / 10.0);
}

