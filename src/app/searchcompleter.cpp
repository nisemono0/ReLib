#include "app/searchcompleter.hpp"

#include <QAbstractItemView>
#include <QScrollBar>


SearchCompleter::SearchCompleter(QObject *parent) : QCompleter(parent) {
    this->completer_model = new QStringListModel(this);

    this->setModel(this->completer_model);

    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    this->setFilterMode(Qt::MatchContains);
    this->setCompletionMode(QCompleter::PopupCompletion);
    this->setMaxVisibleItems(10);

    this->popup()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->popup()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setupDefaultCompleterData();
    this->updateCompletionMode(SearchCompleter::Default);
}

SearchCompleter::~SearchCompleter() {
    delete this->completer_model;
}

void SearchCompleter::setCompleterData(const QList<Manga> &data) {
    for (auto manga : data) {
        if (!this->hasEntry(SearchCompleter::Title, manga.title)) {
            this->insertEntry(SearchCompleter::Title, manga.title);
        }

        for (auto artist : manga.artist) {
            if (!this->hasEntry(SearchCompleter::Artist, artist)) {
                this->insertEntry(SearchCompleter::Artist, artist);
            }
        }

        for (auto parody : manga.parody) {
            if (!this->hasEntry(SearchCompleter::Parody, parody)) {
                this->insertEntry(SearchCompleter::Parody, parody);
            }
        }

        for (auto circle : manga.circle) {
            if (!this->hasEntry(SearchCompleter::Circle, circle)) {
                this->insertEntry(SearchCompleter::Circle, circle);
            }
        }

        for (auto magazine : manga.magazine) {
            if (!this->hasEntry(SearchCompleter::Magazine, magazine)) {
                this->insertEntry(SearchCompleter::Magazine, magazine);
            }
        }

        for (auto event : manga.event) {
            if (!this->hasEntry(SearchCompleter::Event, event)) {
                this->insertEntry(SearchCompleter::Event, event);
            }
        }

        for (auto publisher : manga.publisher) {
            if (!this->hasEntry(SearchCompleter::Publisher, publisher)) {
                this->insertEntry(SearchCompleter::Publisher, publisher);
            }
        }

        for (auto tags : manga.tags) {
            if (!this->hasEntry(SearchCompleter::Tags, tags)) {
                this->insertEntry(SearchCompleter::Tags, tags);
            }
        }
    }
}

QStringList SearchCompleter::splitPath(const QString &path) const {
    if (path.isEmpty()) {
        return QCompleter::splitPath(path);
    }

    // Split on empty space when using the default completer data
    if (this->current_completer_role == SearchCompleter::Default) {
        QString last_item = path.split(" ", Qt::KeepEmptyParts).last().trimmed();
        if (last_item.isEmpty()) {
            return QCompleter::splitPath(path);
        }
        return QStringList(last_item);
    }

    // When using tags split on comma
    QString last_item = path.split(",", Qt::KeepEmptyParts).last().trimmed();
    if (last_item.isEmpty()) {
        return QCompleter::splitPath(path);
    }

    return QStringList(last_item);
}

bool SearchCompleter::isPopupVisible() {
    return this->popup()->isVisible();
}

void SearchCompleter::hidePopup() {
    this->popup()->hide();
}

void SearchCompleter::showPopup() {
    this->complete();
}

void SearchCompleter::selectNextEntry() {
    QModelIndex current_idx = this->popup()->currentIndex();

    int total_rows = this->popup()->model()->rowCount();
    int next_row = (current_idx.row() + 1) % total_rows;

    QModelIndex next_idx = this->popup()->model()->index(next_row, 0);

    this->popup()->setCurrentIndex(next_idx);
}

void SearchCompleter::selectPreviousEntry() {
    QModelIndex current_idx = this->popup()->currentIndex();

    int total_rows = this->popup()->model()->rowCount();
    int previous_row = (current_idx.row() - 1 + total_rows) % total_rows;

    QModelIndex previous_idx = this->popup()->model()->index(previous_row, 0);

    this->popup()->setCurrentIndex(previous_idx);
}

void SearchCompleter::clear() {
    this->setCompletionPrefix("");
    this->popup()->hide();
}

bool SearchCompleter::hasEntry(SearchCompleter::CompleterRole role, const QString &entry) {
    if (this->completer_data[role].contains(entry, Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

void SearchCompleter::insertEntry(SearchCompleter::CompleterRole role, const QString &entry) {
    this->completer_data[role].append(entry);
}

void SearchCompleter::updateCompletionMode(SearchCompleter::CompleterRole role) {
    this->completer_model->setStringList(this->completer_data[role]);
    this->current_completer_role = role;
}

void SearchCompleter::setupDefaultCompleterData() {
    this->completer_data[SearchCompleter::Default] = QStringList({
            QStringLiteral("file_hash:"),
            QStringLiteral("title:"),
            QStringLiteral("artist:"),
            QStringLiteral("parody:"),
            QStringLiteral("circle:"),
            QStringLiteral("magazine:"),
            QStringLiteral("event:"),
            QStringLiteral("publisher:"),
            QStringLiteral("tags:"),
            });
}

void SearchCompleter::receive_updateCompletionMode_request(SearchCompleter::CompleterRole role, const QString &prefix) {
    if (this->current_completer_role != role) {
        this->updateCompletionMode(role);
        this->setCompletionPrefix(prefix);
    } else {
        this->setCompletionPrefix(prefix);
    }
    this->showPopup();
}

