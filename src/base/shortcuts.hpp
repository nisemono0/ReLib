#pragma once

#include <QObject>
#include <QShortcut>


class Shortcuts : public QObject {
    Q_OBJECT;
public:
    explicit Shortcuts(QObject *parent = nullptr);
    ~Shortcuts();

    // MainWindow
    QShortcut *mainwindow_scale_slider_increment;
    QShortcut *mainwindow_scale_slider_decrement;
    QShortcut *mainwindow_refresh_search;

    // LibraryView
    QShortcut *libraryview_next_item;
    QShortcut *libraryview_previous_item;
    QShortcut *libraryview_first_item;
    QShortcut *libraryview_last_item;
    QShortcut *libraryview_select_random;

    // ImageView
    QShortcut *imageview_next_image;
    QShortcut *imageview_previous_image;
    QShortcut *imageview_load_images;
    QShortcut *imageview_jump_to_image;

    // Search
    QShortcut *search_focus_input;
    QShortcut *search_unfocus_input;

};

