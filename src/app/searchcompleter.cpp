#include "app/searchcompleter.hpp"


SearchCompleter::SearchCompleter(QObject *parent) : QCompleter(parent) {
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    this->setFilterMode(Qt::MatchContains);
}

SearchCompleter::~SearchCompleter() {

}

