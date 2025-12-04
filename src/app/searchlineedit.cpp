#include "app/searchlineedit.hpp"


SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent) {
    this->search_completer = new SearchCompleter(this);

    this->setCompleter(this->search_completer);

    this->completer_regex = QRegularExpression(
            QStringLiteral("((?:file_hash:|\\btitle:|\\bartist:|\\bparody:|\\bcircle:|\\bmagazine:|\\bevent:|\\bpublisher:|\\btag:)){([^}]*)}"),
            QRegularExpression::CaseInsensitiveOption);
    this->completer_regex.optimize();

    connect(this, &QLineEdit::textEdited, this, &SearchLineEdit::searchLineEdit_textEdited);
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

bool SearchLineEdit::isCursorInsideBrace(const QString &matched_text, const QString &matched_namespace) {
    int search_from_index = 0;
    int text_idx = 0;
    do {
        text_idx = this->text().indexOf(matched_text, search_from_index);
        int first_brace_pos = text_idx + matched_namespace.length();
        int last_brace_pos = text_idx + matched_text.length() - 1;
        int current_cursor_pos = this->cursorPosition();

        if (first_brace_pos <= current_cursor_pos && current_cursor_pos <= last_brace_pos) {
            return true;
        }

        search_from_index += matched_text.length();
    } while (text_idx != -1);

    return false;
}

SearchCompleter::CompleterRole SearchLineEdit::getCompleterRoleFromNamespace(const QString &matched_namespace) {
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

    if (matched_namespace.compare(QStringLiteral("tag:"), Qt::CaseInsensitive) == 0) {
        return SearchCompleter::Tags;
    }

    return SearchCompleter::Basename;
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

void SearchLineEdit::searchLineEdit_textEdited(const QString &text) {
    for (auto re_match : this->completer_regex.globalMatch(text)) {
        // capture(0) contains the whole matched text, ex: artist:{a1, a2}
        // capture(1) contains the namespace text, ex: artist:
        // capture(2) contains the text inside {}, ex: a1, a2
        if (this->isCursorInsideBrace(re_match.captured(0), re_match.captured(1))) {
            emit request_updateCompletionMode(this->getCompleterRoleFromNamespace(re_match.captured(1)), re_match.captured(2));
            return;
        }
    }
    emit request_updateCompletionMode(SearchCompleter::Disabled, "");
}

