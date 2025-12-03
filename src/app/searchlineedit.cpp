#include "app/searchlineedit.hpp"
#include <qregularexpression.h>


SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent) {
    this->search_completer = new SearchCompleter(this);

    this->setCompleter(this->search_completer);

    connect(this, &QLineEdit::textChanged, this, &SearchLineEdit::searchLineEdit_textChanged);
    connect(this, &SearchLineEdit::request_updateCompletionMode, this->search_completer, &SearchCompleter::receive_updateCompletionMode_request);
}

SearchLineEdit::~SearchLineEdit() {
    delete this->search_completer;
}


void SearchLineEdit::insertMatchingBracket(const QString &bracket) {
    this->insert(bracket);
    this->setCursorPosition(this->cursorPosition() - 1);
}

bool SearchLineEdit::removeMatchingBracket() {
    int cursor_position = this->cursorPosition();
    QString current_text = this->text();

    if (cursor_position == 0 || cursor_position == current_text.length()) {
        return false;
    }

    if (
        (current_text[cursor_position - 1] == QStringLiteral("(") && current_text[cursor_position] == QStringLiteral(")")) ||
        (current_text[cursor_position - 1] == QStringLiteral("[") && current_text[cursor_position] == QStringLiteral("]")) ||
        (current_text[cursor_position - 1] == QStringLiteral("{") && current_text[cursor_position] == QStringLiteral("}"))
       ) {
        this->backspace();
        this->del();
        return true;
    }

    return false;
}

void SearchLineEdit::keyPressEvent(QKeyEvent *event) {
    bool backspace_pressed = false;
    switch (event->key()) {
        case Qt::Key_ParenLeft:
        {
            this->insertMatchingBracket(QStringLiteral(")"));
            break;
        }
        case Qt::Key_BracketLeft:
        {
            this->insertMatchingBracket(QStringLiteral("]"));
            break;
        }
        case Qt::Key_BraceLeft:
        {
            this->insertMatchingBracket(QStringLiteral("}"));
            break;
        }
        case Qt::Key_Backspace:
        {
            backspace_pressed = this->removeMatchingBracket();
            break;
        }
    };

    if (backspace_pressed) {
        return;
    }

    QLineEdit::keyPressEvent(event);
}

void SearchLineEdit::receive_setCompleterData_request(const QList<Manga> &data) {
    this->search_completer->setCompleterData(data);
}

void SearchLineEdit::searchLineEdit_textChanged(const QString &text) {
    // TODO: parse the new text here then emit to the completer to change its mode
    // emit request_setCompleterMode(SearchCompleter::Artist, "hi");
    // QRegularExpression contextRegex(R"((\w+):\{([^}]*)\})");
    // QRegularExpressionMatch match = contextRegex.match(text);

    // qDebug() << match;

}

