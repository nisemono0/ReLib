#pragma once

#include "utils/defs.hpp"

#include <QString>
#include <QSettings>


class Settings {
public:
    static bool scale_image;
    static int scale_slider_value;
    static float image_scale_value;

    static ImageOptions::ImageOptions imageview_option;

    static bool autocomplete_search;
    static bool search_while_typing;
    static bool select_first_item;

    static bool remember_settings;

    static QString last_database_path;

    static QString last_add_file_dialog_path;
    static QString last_add_dir_dialog_path;
    static QString last_select_database_dialog_path;


    static void saveSettings();
    static void loadSettings();

    static void updateImageScaleValue();

private:
    static QSettings settings;
    static void loadDefaultSettings();

};
