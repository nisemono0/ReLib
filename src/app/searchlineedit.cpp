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

