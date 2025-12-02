#pragma once

#include <QObject>
#include <QShortcut>


class Shortcuts : public QObject {
    Q_OBJECT;
public:
    explicit Shortcuts(QObject *parent = nullptr);
    ~Shortcuts();

    // LibraryView
    QShortcut *libraryview_next_item;
    QShortcut *libraryview_previous_item;
    // ImageView
    QShortcut *imageview_next_image;
    QShortcut *imageview_previous_image;
    QShortcut *imageview_load_images;
    // Search
    QShortcut *focus_search_input;

};

