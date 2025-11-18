#include "app/librarymodelproxy.hpp"
#include "app/librarymodel.hpp"

#include <QRandomGenerator>


LibraryModelProxy::LibraryModelProxy(QObject *parent) : QSortFilterProxyModel(parent) {
    this->search_text = "";

    this->search_regex = QRegularExpression(
            QStringLiteral("(?:file_hash:|\\btitle:|\\bartist:|\\bparody:|\\bcircle:|\\bmagazine:|\\bevent:|\\bpublisher:|\\btag:){([^}]+)}"),
            QRegularExpression::CaseInsensitiveOption);
    this->search_regex.optimize();

    this->sort_collator = new QCollator();
    this->sort_collator->setNumericMode(true);
    this->sort_collator->setCaseSensitivity(Qt::CaseInsensitive);

    this->setDynamicSortFilter(true);
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->sort(Qt::AscendingOrder);
}

LibraryModelProxy::~LibraryModelProxy() {
    delete this->sort_collator;
}

void LibraryModelProxy::setSearchText(const QString &search_text) {
    beginFilterChange();
    this->search_text = search_text;
    endFilterChange();
}

QModelIndex LibraryModelProxy::getFirstIndex() {
    return this->index(0, 0);
}

QModelIndex LibraryModelProxy::getRandomIndex() {
    int total_visible = this->rowCount();
    if (total_visible > 0) {
        return this->index(QRandomGenerator::global()->bounded(0, total_visible), 0);
    }
    return this->index(0, 0);
}

bool LibraryModelProxy::searchItemData(const QStringList &search_list, const QStringList &data_list) const {
    for (auto search_string : search_list) {
        for (auto data_item : data_list) {
            if (data_item.contains(search_string.trimmed(), Qt::CaseInsensitive)) {
                return true;
            }
        }
    }
    return false;
}

bool LibraryModelProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    QModelIndex idx = this->sourceModel()->index(source_row, 0, source_parent);
    QAbstractItemModel *item = this->sourceModel();

    QString search_string = this->search_text;

    bool is_item_found = true;

    for (auto re_match : this->search_regex.globalMatch(search_string)) {
        // capture(0) contains the whole matched text, ex: artist:{a1, a2}
        // capture(1) contains the text inside {}, ex: a1, a2
        QString match_around = re_match.captured(0);
        search_string = search_string.remove(match_around, Qt::CaseSensitive).trimmed();

        // If we search by hash we dont need to check for other types, just return
        if (match_around.contains(QStringLiteral("file_hash:"), Qt::CaseInsensitive)) {
            return this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::FileHash).toStringList());
        }

        // TODO(maybe): Add flag to search by OR instead of AND
        if (match_around.contains(QStringLiteral("title:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Title).toStringList());
        }

        if (match_around.contains(QStringLiteral("artist:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Artist).toStringList());
        }

        if (match_around.contains(QStringLiteral("parody:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Parody).toStringList());
        }

        if (match_around.contains(QStringLiteral("circle:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Circle).toStringList());
        }

        if (match_around.contains(QStringLiteral("magazine:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Magazine).toStringList());
        }

        if (match_around.contains(QStringLiteral("event:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Event).toStringList());
        }

        if (match_around.contains(QStringLiteral("publisher:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Publisher).toStringList());
        }

        if (match_around.contains(QStringLiteral("tag:"), Qt::CaseInsensitive)) {
            is_item_found = is_item_found && this->searchItemData(re_match.captured(1).split(",", Qt::SkipEmptyParts), item->data(idx, LibraryModel::Tags).toStringList());
        }
    }

    return is_item_found && item->data(idx).toString().contains(search_string, Qt::CaseInsensitive);
}

bool LibraryModelProxy::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    return this->sort_collator->compare(source_left.data().toString(), source_right.data().toString()) <= 0;
}

