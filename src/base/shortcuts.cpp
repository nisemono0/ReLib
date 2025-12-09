#include "base/shortcuts.hpp"


Shortcuts::Shortcuts(QObject *parent) : QObject(parent) {
    // MainWindow
    this->mainwindow_scale_slider_increment = new QShortcut(parent);
    this->mainwindow_scale_slider_increment->setKeys({
            QKeySequence(Qt::ControlModifier | Qt::Key_Equal)
            });
    this->mainwindow_scale_slider_decrement = new QShortcut(parent);
    this->mainwindow_scale_slider_decrement->setKeys({
            QKeySequence(Qt::ControlModifier | Qt::Key_Minus)
            });
    this->mainwindow_refresh_search = new QShortcut(parent);
    this->mainwindow_refresh_search->setKeys({
            QKeySequence(Qt::ControlModifier | Qt::Key_C)
            });

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
    this->libraryview_first_item->setKeys({
            QKeySequence(Qt::Key_G, Qt::Key_G)
            });
    this->libraryview_last_item = new QShortcut(parent);
    this->libraryview_last_item->setKeys({
            QKeySequence(Qt::ShiftModifier | Qt::Key_G)
            });
    this->libraryview_select_random = new QShortcut(parent);
    this->libraryview_select_random->setKeys({
            QKeySequence(Qt::ControlModifier | Qt::Key_R)
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
    this->imageview_load_images->setKeys({
            Qt::Key_O
            });
    this->imageview_jump_to_image = new QShortcut(parent);
    this->imageview_jump_to_image->setKeys({
            QKeySequence(Qt::Key_G, Qt::Key_I)
            });

    // Search
    this->search_focus_input = new QShortcut(parent);
    this->search_focus_input->setKeys({
            QKeySequence(Qt::ControlModifier | Qt::Key_F),
            Qt::Key_Slash
            });
    this->search_unfocus_input = new QShortcut(parent);
    this->search_unfocus_input->setKeys({
            Qt::Key_Escape
            });
}

Shortcuts::~Shortcuts() {
    delete this->mainwindow_scale_slider_increment;
    delete this->mainwindow_scale_slider_decrement;
    delete this->mainwindow_refresh_search;

    delete this->libraryview_next_item;
    delete this->libraryview_previous_item;
    delete this->libraryview_first_item;
    delete this->libraryview_last_item;
    delete this->libraryview_select_random;

    delete this->imageview_next_image;
    delete this->imageview_previous_image;
    delete this->imageview_load_images;
    delete this->imageview_jump_to_image;

    delete this->search_focus_input;
    delete this->search_unfocus_input;
}

