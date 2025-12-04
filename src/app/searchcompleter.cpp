#include "app/searchcompleter.hpp"


SearchCompleter::SearchCompleter(QObject *parent) : QCompleter(parent) {
    this->completer_model = new QStringListModel(this);

    this->setModel(this->completer_model);

    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    this->setFilterMode(Qt::MatchContains);
    this->setCompletionMode(QCompleter::PopupCompletion);

    this->current_completer_role = SearchCompleter::Disabled;
}

SearchCompleter::~SearchCompleter() {
    delete this->completer_model;
}

void SearchCompleter::setCompleterData(const QList<Manga> &data) {
    for (auto manga : data) {
        if (!this->hasEntry(SearchCompleter::Basename, manga.file_basename)) {
            this->insertEntry(SearchCompleter::Basename, manga.file_basename);
        }

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

void SearchCompleter::receive_updateCompletionMode_request(SearchCompleter::CompleterRole role, const QString &prefix) {
    if (this->current_completer_role != role) {
        this->updateCompletionMode(role);
        this->setCompletionPrefix(prefix);
    } else {
        this->setCompletionPrefix(prefix);
    }
    this->complete();
}

