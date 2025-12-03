#include "app/searchlineedit.hpp"


SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent) {
    this->setCompleter(this->search_completer);
}

SearchLineEdit::~SearchLineEdit() {
    delete this->search_completer;
}

