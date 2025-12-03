#include "base/shortcuts.hpp"


Shortcuts::Shortcuts(QObject *parent) : QObject(parent) {
    // MainWindow
    this->mainwindow_scale_slider_increment = new QShortcut(parent);
    this->mainwindow_scale_slider_increment->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_Equal)
            });
    this->mainwindow_scale_slider_decrement = new QShortcut(parent);
    this->mainwindow_scale_slider_decrement->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_Minus)
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

    // Search
    this->focus_search_input = new QShortcut(parent);
    this->focus_search_input->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_F),
            Qt::Key_Slash
            });
    this->unfocus_search_input = new QShortcut(parent);
    this->unfocus_search_input->setKeys({
            Qt::Key_Escape
            });
    this->search_move_char_forward = new QShortcut(parent);
    this->search_move_char_forward->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_L)
            });
    this->search_move_char_backward = new QShortcut(parent);
    this->search_move_char_backward->setKeys({
            QKeySequence(Qt::CTRL | Qt::Key_H)
            });
    this->search_move_word_forward = new QShortcut(parent);
    this->search_move_word_forward->setKeys({
            QKeySequence(Qt::ALT | Qt::Key_L)
            });
    this->search_move_word_backward = new QShortcut(parent);
    this->search_move_word_backward->setKeys({
            QKeySequence(Qt::ALT | Qt::Key_H)
            });

}

Shortcuts::~Shortcuts() {
    delete this->mainwindow_scale_slider_increment;
    delete this->mainwindow_scale_slider_decrement;

    delete this->libraryview_next_item;
    delete this->libraryview_previous_item;
    delete this->libraryview_first_item;
    delete this->libraryview_last_item;

    delete this->imageview_next_image;
    delete this->imageview_previous_image;
    delete this->imageview_load_images;

    delete this->focus_search_input;
    delete this->unfocus_search_input;
    delete this->search_move_char_forward;
    delete this->search_move_char_backward;
    delete this->search_move_word_forward;
    delete this->search_move_word_backward;
}

