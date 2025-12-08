#include "app/searchplaintextedit.hpp"

#include "base/settings.hpp"


SearchPlainTextEdit::SearchPlainTextEdit(QWidget *parent) : QPlainTextEdit(parent) {
    this->setCompleter(new SearchCompleter(this));

    this->completer_regex = QRegularExpression(
            QStringLiteral("((?:file_hash:|\\btitle:|\\bartist:|\\bparody:|\\bcircle:|\\bmagazine:|\\bevent:|\\bpublisher:|\\btags:)){([^}]+)}"),
            QRegularExpression::CaseInsensitiveOption);
    this->completer_regex.optimize();

    // Plain text edit
    connect(this, &QPlainTextEdit::textChanged, this, &SearchPlainTextEdit::searchPlainTextEdit_textEdited);
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

void SearchPlainTextEdit::selectNextCompletion() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->selectNextEntry();
    }
}

void SearchPlainTextEdit::selectPreviousCompletion() {
    if (this->search_completer->isPopupVisible()) {
        this->search_completer->selectPreviousEntry();
    }
}

void SearchPlainTextEdit::clearCompleter() {
    this->search_completer->clear();
}

void SearchPlainTextEdit::insertMatchingBracket(const QString &bracket) {
    // TODO
    // this->insert(bracket);
    // this->setCursorPosition(this->cursorPosition() - 1);
}

bool SearchPlainTextEdit::removeMatchingBracket() {
    // TODO
    // int cursor_position = this->cursorPosition();
    // QString current_text = this->text();

    // if (cursor_position == 0 || cursor_position == current_text.length()) {
    //     return false;
    // }

    // if (
    //     (current_text[cursor_position - 1] == QStringLiteral("(") && current_text[cursor_position] == QStringLiteral(")")) ||
    //     (current_text[cursor_position - 1] == QStringLiteral("[") && current_text[cursor_position] == QStringLiteral("]")) ||
    //     (current_text[cursor_position - 1] == QStringLiteral("{") && current_text[cursor_position] == QStringLiteral("}"))
    //    ) {
    //     this->backspace();
    //     this->del();
    //     return true;
    // }

    // return false;
}

bool SearchPlainTextEdit::isCursorInsideBrace(const QString &whole_text, const QString &namespace_text, const QString &tags_text) {
    // TODO
    // current_cursor_pos = this->cursorPosition();

    // int namespace_len = namespace_text.length();
    // int tags_len = tags_text.length();

    // QString line_text = this->text();

    // int whole_text_start_pos = line_text.indexOf(whole_text);
    // while (whole_text_start_pos != -1) {
    //     int namespace_start_pos = whole_text_start_pos + namespace_len;
    //     int namespace_end_pos = namespace_start_pos + tags_len + 1;

    //     if (namespace_start_pos < current_cursor_pos && current_cursor_pos <= namespace_end_pos) {
    //         return true;
    //     }
    //     whole_text_start_pos = line_text.indexOf(whole_text, namespace_end_pos);
    // }
    // return false;
}

void SearchPlainTextEdit::jumpOrInsertClosedBracket(const QString &bracket) {
    // TODO
    // int current_cursor_pos = this->cursorPosition();
    // if (this->text()[current_cursor_pos] == bracket) {
    //     this->setCursorPosition(this->cursorPosition() + 1);
    //     this->search_completer->hidePopup();
    // } else {
    //     this->insert(bracket);
    // }
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

void SearchPlainTextEdit::keyPressEvent(QKeyEvent *event) {
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
        // Simulate QLineEdit returnPressed signal
        case Qt::Key_Return:
        {
            emit returnPressed();
            return;
        }
    };

    QPlainTextEdit::keyPressEvent(event);
}

void SearchPlainTextEdit::receive_setCompleterData_request(const QList<Manga> &data) {
    this->search_completer->setCompleterData(data);
}

void SearchPlainTextEdit::searchPlainTextEdit_textEdited() {
    // TODO
    // if (!Settings::autocomplete_search) {
    //     return;
    // }

    // for (auto re_match : this->completer_regex.globalMatch(text)) {
    //     // capture(0) contains the whole matched text, ex: artist:{a1, a2}
    //     // capture(1) contains the namespace text, ex: artist:
    //     // capture(2) contains the text inside {}, ex: a1, a2
    //     QString whole_text = re_match.captured(0);
    //     QString namepsace_text = re_match.captured(1);
    //     QString tags_text = re_match.captured(2);
    //     if (this->isCursorInsideBrace(whole_text, namepsace_text, tags_text)) {
    //         emit request_updateCompletionMode(this->getCompleterRoleFromNamespace(re_match.captured(1)), re_match.captured(2));
    //         return;
    //     }
    // }

    // emit request_updateCompletionMode(SearchCompleter::Default, text);
}

void SearchPlainTextEdit::searchPlainTextEdit_cursorPositionChanged() {

    // Same as searchPlainTextEdit_textEdited but uses new_pos
    // Hides the completion popup if cursor is outside braces
    // TODO
    // for (auto re_match : this->completer_regex.globalMatch(this->text())) {
    //     QString whole_text = re_match.captured(0);
    //     QString namespace_text = re_match.captured(1);
    //     QString tags_text = re_match.captured(2);
    //     if (!this->isCursorInsideBrace(whole_text, namespace_text, tags_text)) {
    //         this->search_completer->hidePopup();
    //     }
    // }

}

void SearchPlainTextEdit::receive_completerText(const QString &completer_text) {
    // TODO
    // if (this->search_completer->widget() != this) {
    //     return;
    // }

    // QString last_prefix = this->search_completer->splitPath(
    //         this->search_completer->completionPrefix()
    //         ).last();
    // if (last_prefix.isEmpty()) {
    //     return;
    // }

    // int replace_last_pos = this->text().lastIndexOf(last_prefix, Qt::CaseSensitive);
    // if (replace_last_pos == -1) {
    //     return;
    // }

    // int prefix_len = last_prefix.length();
    // int current_cursor_pos = this->cursorPosition();
    // int completer_text_len = completer_text.length();

    // QString new_text = this->text().replace(replace_last_pos, prefix_len, completer_text);
    // int new_cursor_pos = current_cursor_pos - prefix_len + completer_text_len;

    // if (this->search_completer->isDefaultCompletion()) {
    //     new_cursor_pos -= 1;
    // }

    // this->setText(new_text);
    // this->setCursorPosition(new_cursor_pos);
}

