#include "app/searchplaintextedit.hpp"

#include "base/settings.hpp"

#include <QApplication>


SearchPlainTextEdit::SearchPlainTextEdit(QWidget *parent) : QPlainTextEdit(parent) {
    this->default_stylesheet = this->styleSheet();
    this->focused_stylesheet = QStringLiteral("QPlainTextEdit { border: 1px solid dimgray }");

    this->setCompleter(new SearchCompleter(this));

    this->completer_regex = QRegularExpression(
            QStringLiteral("((?:file_hash:|\\btitle:|\\bartist:|\\bparody:|\\bcircle:|\\bmagazine:|\\bevent:|\\bpublisher:|\\btags:)){([^}]+)}"),
            QRegularExpression::CaseInsensitiveOption);
    this->completer_regex.optimize();

    // Plain text edit
    connect(this, &QPlainTextEdit::textChanged, this, &SearchPlainTextEdit::searchPlainTextEdit_textChanged);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &SearchPlainTextEdit::searchPlainTextEdit_cursorPositionChanged);

    // Completer
    connect(this, &SearchPlainTextEdit::request_updateCompletionMode, this->search_completer, &SearchCompleter::receive_updateCompletionMode_request);
}

SearchPlainTextEdit::~SearchPlainTextEdit() {
    delete this->search_completer;
}

void SearchPlainTextEdit::setCompleter(SearchCompleter *search_completer) {
    if (this->search_completer) {
        this->search_completer->disconnect(this);
        delete this->search_completer;
    }

    this->search_completer = search_completer;

    if (!search_completer) {
        return;
    }

    this->search_completer->setWidget(this);

    connect(this->search_completer, qOverload<const QString&>(&QCompleter::activated), this, &SearchPlainTextEdit::receive_completerText, Qt::UniqueConnection);
}

SearchCompleter* SearchPlainTextEdit::completer() const {
    return this->search_completer;
}

void SearchPlainTextEdit::hideCompleter() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->hidePopup();
    }
}

bool SearchPlainTextEdit::isCompleterVisible() {
    return this->search_completer->isPopupVisible();
}

void SearchPlainTextEdit::clearCompleter() {
    this->search_completer->clear();
}

void SearchPlainTextEdit::insertMatchingBracket(const QString &bracket) {
    QTextCursor text_cursor = this->textCursor();
    text_cursor.insertText(bracket);
    text_cursor.movePosition(QTextCursor::PreviousCharacter);
    this->setTextCursor(text_cursor);
}

bool SearchPlainTextEdit::removeMatchingBracket() {
    QTextCursor text_cursor = this->textCursor();
    int cursor_position = text_cursor.position();
    QString current_text = this->toPlainText();

    if (text_cursor.atStart() || text_cursor.atEnd()) {
        return false;
    }

    if (
        (current_text[cursor_position - 1] == QStringLiteral("(") && current_text[cursor_position] == QStringLiteral(")")) ||
        (current_text[cursor_position - 1] == QStringLiteral("[") && current_text[cursor_position] == QStringLiteral("]")) ||
        (current_text[cursor_position - 1] == QStringLiteral("{") && current_text[cursor_position] == QStringLiteral("}"))
       ) {
        text_cursor.deletePreviousChar();
        text_cursor.deleteChar();
        return true;
    }

    return false;
}

bool SearchPlainTextEdit::isCursorInsideBrace(const QString &whole_text, const QString &namespace_text, const QString &tags_text) {
    QTextCursor text_cursor = this->textCursor();
    int current_cursor_pos = text_cursor.position();

    int namespace_len = namespace_text.length();
    int tags_len = tags_text.length();

    QString text = this->toPlainText();

    int whole_text_start_pos = text.indexOf(whole_text);
    while (whole_text_start_pos != -1) {
        int namespace_start_pos = whole_text_start_pos + namespace_len;
        int namespace_end_pos = namespace_start_pos + tags_len + 1;

        if (namespace_start_pos < current_cursor_pos && current_cursor_pos <= namespace_end_pos) {
            return true;
        }
        whole_text_start_pos = text.indexOf(whole_text, namespace_end_pos);
    }
    return false;
}

void SearchPlainTextEdit::jumpOrInsertClosedBracket(const QString &bracket) {
    QString current_text = this->toPlainText();

    QTextCursor text_cursor = this->textCursor();
    int cursor_pos = text_cursor.position();

    if (current_text[cursor_pos] == bracket) {
        text_cursor.movePosition(QTextCursor::NextCharacter);
        this->setTextCursor(text_cursor);
        this->search_completer->hidePopup();
    } else {
        text_cursor.insertText(bracket);
    }
}

SearchCompleter::CompleterRole SearchPlainTextEdit::getCompleterRoleFromNamespace(const QString &matched_namespace) {
    if (matched_namespace.compare(QStringLiteral("title:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Title;
    }

    if (matched_namespace.compare(QStringLiteral("artist:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Artist;
    }

    if (matched_namespace.compare(QStringLiteral("parody:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Parody;
    }

    if (matched_namespace.compare(QStringLiteral("circle:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Circle;
    }

    if (matched_namespace.compare(QStringLiteral("magazine:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Magazine;
    }

    if (matched_namespace.compare(QStringLiteral("publisher:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Publisher;
    }

    if (matched_namespace.compare(QStringLiteral("tags:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Tags;
    }

    return SearchCompleter::Title;
}

void SearchPlainTextEdit::focusInEvent(QFocusEvent *event) {
    // Receive keyboard focus events for the widget
    if (this->search_completer) {
        this->search_completer->setWidget(this);
    }

    this->setStyleSheet(this->focused_stylesheet);

    QPlainTextEdit::focusInEvent(event);
}

void SearchPlainTextEdit::focusOutEvent(QFocusEvent *event) {
    this->setStyleSheet(this->default_stylesheet);

    QPlainTextEdit::focusOutEvent(event);
}

void SearchPlainTextEdit::keyPressEvent(QKeyEvent *event) {
    // Disable the QPlainTextEdit keys and let them be
    // handled by the completion popup if it's visible
    if (this->search_completer && this->search_completer->isPopupVisible()) {
        switch (event->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            {
                event->ignore();
                return;
            }
            case Qt::Key_Tab:
            {
                this->search_completer->selectNextEntry();
                return;
            }
            case Qt::Key_Backtab:
            {
                this->search_completer->selectPreviousEntry();
                return;
            }
            case Qt::Key_J:
            {
                if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                    this->search_completer->selectNextEntry();
                    return;
                }
                break;
            }
            case Qt::Key_K:
            {
                if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                    this->search_completer->selectPreviousEntry();
                    return;
                }
                break;
            }
            default:
                break;
        }
    }

    switch (event->key()) {
        // Insert matching/jump over/delete matching bracket
        case Qt::Key_ParenLeft:
        {
            this->insertMatchingBracket(QStringLiteral(")"));
            break;
        }
        case Qt::Key_ParenRight:
        {
            this->jumpOrInsertClosedBracket(QStringLiteral(")"));
            return;
        }
        case Qt::Key_BracketLeft:
        {
            this->insertMatchingBracket(QStringLiteral("]"));
            break;
        }
        case Qt::Key_BracketRight:
        {
            this->jumpOrInsertClosedBracket(QStringLiteral("]"));
            return;
        }
        case Qt::Key_BraceLeft:
        {
            this->insertMatchingBracket(QStringLiteral("}"));
            break;
        }
        case Qt::Key_BraceRight:
        {
            this->jumpOrInsertClosedBracket(QStringLiteral("}"));
            return;
        }
        case Qt::Key_Backspace:
        {
            if (this->removeMatchingBracket()) {
                return;
            }
            break;
        }
        // SearchPlainTextEdit cursor movement
        case Qt::Key_L:
        {
            if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                this->moveCursor(QTextCursor::Right);
                return;
            } else if (QApplication::keyboardModifiers() == Qt::AltModifier) {
                this->moveCursor(QTextCursor::WordRight);
                return;
            }
            break;
        }
        case Qt::Key_H:
        {
            if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
                this->moveCursor(QTextCursor::Left);
                return;
            } else if (QApplication::keyboardModifiers() == Qt::AltModifier) {
                this->moveCursor(QTextCursor::WordLeft);
                return;
            }
            break;
        }
        // Simulate QLineEdit returnPressed signal
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            emit returnPressed();
            return;
        }
        default:
            break;
    };

    QPlainTextEdit::keyPressEvent(event);
}

void SearchPlainTextEdit::receive_setCompleterData_request(const QList<Manga> &data) {
    this->search_completer->setCompleterData(data);
}

void SearchPlainTextEdit::searchPlainTextEdit_textChanged() {
    if (!Settings::autocomplete_search) {
        return;
    }

    QString text = this->toPlainText();

    for (auto re_match : this->completer_regex.globalMatch(text)) {
        // capture(0) contains the whole matched text, ex: artist:{a1, a2}
        // capture(1) contains the namespace text, ex: artist:
        // capture(2) contains the text inside {}, ex: a1, a2
        QString whole_text = re_match.captured(0);
        QString namepsace_text = re_match.captured(1);
        QString tags_text = re_match.captured(2);
        if (this->isCursorInsideBrace(whole_text, namepsace_text, tags_text)) {
            emit request_updateCompletionMode(this->getCompleterRoleFromNamespace(re_match.captured(1)), re_match.captured(2));
            return;
        }
    }

    emit request_updateCompletionMode(SearchCompleter::Default, text);
}

void SearchPlainTextEdit::searchPlainTextEdit_cursorPositionChanged() {
    // Same as searchPlainTextEdit_textChanged but hides
    // the completion popup if cursor is outside braces

    bool is_inside_brace = false;

    QString text = this->toPlainText();
    for (auto re_match : this->completer_regex.globalMatch(text)) {
        QString whole_text = re_match.captured(0);
        QString namespace_text = re_match.captured(1);
        QString tags_text = re_match.captured(2);
        if (this->isCursorInsideBrace(whole_text, namespace_text, tags_text)) {
            is_inside_brace = true;
            break;
        }
    }

    if (!is_inside_brace && this->search_completer->isPopupVisible()) {
        this->search_completer->hidePopup();
    }
}

void SearchPlainTextEdit::receive_completerText(const QString &completer_text) {
    if (this->search_completer->widget() != this) {
        return;
    }

    QString last_prefix = this->search_completer->splitPath(
            this->search_completer->completionPrefix()
            ).last();
    if (last_prefix.isEmpty()) {
        return;
    }

    QTextCursor text_cursor = this->textCursor();

    text_cursor.movePosition(QTextCursor::Left);
    text_cursor.select(QTextCursor::WordUnderCursor);

    text_cursor.insertText(completer_text);

    if (this->search_completer->isDefaultCompletion()) {
        text_cursor.movePosition(QTextCursor::Left);
    }

    this->setTextCursor(text_cursor);

    this->search_completer->hidePopup();
}

