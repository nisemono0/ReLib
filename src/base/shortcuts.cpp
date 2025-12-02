#include "base/shortcuts.hpp"


Shortcuts::Shortcuts(QObject *parent) : QObject(parent) {
    // LibraryView
    this->libraryview_next_item = new QShortcut(parent);
    this->libraryview_next_item->setKeys({
            Qt::Key_Down,
            Qt::Key_J
            });
    this->libraryview_previous_item = new QShortcut(parent);
    this->libraryview_previous_item->setKeys({
            Qt::Key_Up,
            Qt::Key_K
            });
    this->libraryview_first_item = new QShortcut(parent);
    this->libraryview_first_item->setKey({
            QKeySequence(Qt::Key_G, Qt::Key_G)
            });
    this->libraryview_last_item = new QShortcut(parent);
    this->libraryview_last_item->setKey({
            QKeySequence(Qt::ShiftModifier | Qt::Key_G)
            });
    // ImageView
    this->imageview_next_image = new QShortcut(parent);
    this->imageview_next_image->setKeys({
            Qt::Key_Right,
            Qt::Key_L
            });
    this->imageview_previous_image = new QShortcut(parent);
    this->imageview_previous_image->setKeys({
            Qt::Key_Left,
            Qt::Key_H
            });
    this->imageview_load_images = new QShortcut(parent);
    this->imageview_load_images->setKey(
            Qt::Key_O
            );
    // Search
    this->focus_search_input = new QShortcut(parent);
    this->focus_search_input->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_F),
            Qt::Key_Slash
            });
}

Shortcuts::~Shortcuts() {
    delete this->libraryview_next_item;
    delete this->libraryview_previous_item;
    delete this->libraryview_first_item;
    delete this->libraryview_last_item;

    delete this->imageview_next_image;
    delete this->imageview_previous_image;
    delete this->imageview_load_images;

    delete this->focus_search_input;
}

