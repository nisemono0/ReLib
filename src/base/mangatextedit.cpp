#include "base/mangatextedit.hpp"

#include "base/clipboard.hpp"


MangaTextEdit::MangaTextEdit(QWidget *parent) : QPlainTextEdit(parent) {
    this->initContextMenu();

    connect(this->copy_action, &QAction::triggered, this, &MangaTextEdit::copy_action_triggered);
}

MangaTextEdit::~MangaTextEdit() {
    delete this->copy_action;
    delete this->context_menu;
}

void MangaTextEdit::initContextMenu() {
    this->context_menu = new QMenu(QStringLiteral("Text menu"), this);
    this->copy_action = new QAction(QStringLiteral("Copy text"), this);

    this->context_menu->addAction(this->copy_action);
}

void MangaTextEdit::contextMenuEvent(QContextMenuEvent *event) {
    this->context_menu->popup(event->globalPos());
    QPlainTextEdit::contextMenuEvent(event);
}

void MangaTextEdit::copy_action_triggered() {
    QString plain_text = this->toPlainText();
    if (plain_text.isEmpty()) {
        return;
    }
    Clipboard::setText(plain_text);
}

