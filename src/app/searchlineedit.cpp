#include "app/searchlineedit.hpp"


SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent) {
    this->setCompleter(new SearchCompleter(this));

    this->completer_regex = QRegularExpression(
            QStringLiteral("((?:file_hash:|\\btitle:|\\bartist:|\\bparody:|\\bcircle:|\\bmagazine:|\\bevent:|\\bpublisher:|\\btag:)){([^}]+)}"),
            QRegularExpression::CaseInsensitiveOption);
    this->completer_regex.optimize();

    // Line edit
    connect(this, &QLineEdit::textEdited, this, &SearchLineEdit::searchLineEdit_textEdited);
    // Completer
    connect(this, &SearchLineEdit::request_updateCompletionMode, this->search_completer, &SearchCompleter::receive_updateCompletionMode_request);
}

SearchLineEdit::~SearchLineEdit() {
    delete this->search_completer;
}

void SearchLineEdit::setCompleter(SearchCompleter *search_completer) {
    if (this->search_completer) {
        this->search_completer->disconnect(this);
        delete this->search_completer;
    }

    this->search_completer = search_completer;

    if (!search_completer) {
        return;
    }

    this->search_completer->setWidget(this);

    connect(this->search_completer, qOverload<const QString&>(&QCompleter::activated), this, &SearchLineEdit::receive_completerText, Qt::UniqueConnection);
}

SearchCompleter* SearchLineEdit::completer() const {
    return this->search_completer;
}

void SearchLineEdit::hideCompleter() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->hidePopup();
    }
}

bool SearchLineEdit::isCompleterVisible() {
    return this->search_completer->isPopupVisible();
}

void SearchLineEdit::selectNextCompletion() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->selectNextEntry();
    }
}

void SearchLineEdit::selectPreviousCompletion() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->selectPreviousEntry();
    }
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

void SearchLineEdit::jumpOrInsertClosedBracket(const QString &bracket) {
    int current_cursor_pos = this->cursorPosition();
    if (this->text()[current_cursor_pos] == bracket) {
        this->setCursorPosition(this->cursorPosition() + 1);
        this->search_completer->hidePopup();
    } else {
        this->insert(bracket);
    }
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
    switch (event->key()) {
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
        }
    };

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

void SearchLineEdit::receive_completerText(const QString &completer_text) {
    if (this->search_completer->widget() != this) {
        return;
    }

    QString last_prefix = this->search_completer->splitPath(
            this->search_completer->completionPrefix()
            ).last();
    if (last_prefix.isEmpty()) {
        return;
    }

    int replace_last_pos = this->text().lastIndexOf(last_prefix, Qt::CaseSensitive);
    if (replace_last_pos == -1) {
        return;
    }

    int prefix_len = last_prefix.length();
    int current_cursor_pos = this->cursorPosition();
    int completer_text_len = completer_text.length();

    QString new_text = this->text().replace(replace_last_pos, prefix_len, completer_text);
    int new_cursor_pos = current_cursor_pos - prefix_len + completer_text_len;

    this->setText(new_text);
    this->setCursorPosition(new_cursor_pos);
}

