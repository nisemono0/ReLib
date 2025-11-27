#include "app/libraryview.hpp"

#include <base/clipboard.hpp>
#include <base/settings.hpp>

#include "utils/str.hpp"
#include "utils/fs.hpp"

#include <QMenu>


LibraryView::LibraryView(QWidget *parent) : QListView(parent) {
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->initContextMenu();

    this->library_model = new LibraryModel(this);

    this->library_model_proxy = new LibraryModelProxy(this);
    this->library_model_proxy->setSourceModel(this->library_model);

    this->setModel(this->library_model_proxy);

    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &LibraryView::libraryView_selectionModel_currentChanged);

    connect(this->show_info_action, &QAction::triggered, this, &LibraryView::show_info_action_triggered);
    connect(this->copy_item_name_action, &QAction::triggered, this, &LibraryView::copy_item_name_action_triggered);
    connect(this->copy_title_action, &QAction::triggered, this, &LibraryView::copy_title_action_triggered);
    connect(this->copy_path_action, &QAction::triggered, this, &LibraryView::copy_path_action_triggered);
    connect(this->copy_hash_action, &QAction::triggered, this, &LibraryView::copy_hash_action_triggered);
    connect(this->copy_tags_action, &QAction::triggered, this, &LibraryView::copy_tags_action_triggered);
    connect(this->remove_manga_action, &QAction::triggered, this, &LibraryView::remove_manga_action_triggered);

}

LibraryView::~LibraryView() {
    delete this->library_model_proxy;
    delete this->library_model;

    delete this->show_info_action;
    delete this->copy_item_name_action;
    delete this->copy_title_action;
    delete this->copy_path_action;
    delete this->copy_hash_action;
    delete this->copy_tags_action;
    delete this->remove_manga_action;
    delete this->context_menu;
    delete this->copy_context_menu;
}

void LibraryView::initContextMenu() {
    this->context_menu = new QMenu("Library menu", this);
    this->copy_context_menu = new QMenu("Copy", this);

    this->show_info_action = new QAction("Show info", this);
    this->copy_item_name_action = new QAction("Copy item name", this);

    this->copy_title_action = new QAction("Copy title", this);
    this->copy_path_action = new QAction("Copy path", this);
    this->copy_hash_action = new QAction("Copy hash", this);
    this->copy_tags_action = new QAction("Copy tags", this);
    this->remove_manga_action = new QAction("Remove from database", this);

    this->context_menu->addAction(this->show_info_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->copy_item_name_action);
    this->context_menu->addSeparator();

    this->context_menu->addMenu(this->copy_context_menu);
    this->copy_context_menu->addAction(this->copy_title_action);
    this->copy_context_menu->addAction(this->copy_path_action);
    this->copy_context_menu->addAction(this->copy_hash_action);
    this->copy_context_menu->addAction(this->copy_tags_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->remove_manga_action);
}

void LibraryView::updateLibraryViewStatus() {
    if (Utils::Str::isNullOrEmpty(this->current_item_filesize)) {
        emit send_LibraryView_status(QStringLiteral("Showing: [%1/%2]").arg(
                    QString::number(this->library_model_proxy->rowCount()),
                    QString::number(this->library_model->rowCount())
                    ));
    } else {
        emit send_LibraryView_status(QStringLiteral("Showing: [%1/%2] | Filesize: [%3]").arg(
                    QString::number(this->library_model_proxy->rowCount()),
                    QString::number(this->library_model->rowCount()),
                    this->current_item_filesize
                    ));
    }

}

QString LibraryView::getSelectedItemsData(LibraryModel::LibraryRole role) {
    QModelIndexList selected_idxs = this->selectedIndexes();
    if (selected_idxs.isEmpty()) {
        return QString();
    } else if (selected_idxs.length() > 1) {
        QString item_data = "";
        for (auto idx : selected_idxs) {
            item_data.append(QStringLiteral("%1\n").arg(idx.data(role).toStringList().join("; ")));
        }
        return item_data;
    } else {
        return selected_idxs.first().data(role).toString();
    }
}

void LibraryView::contextMenuEvent(QContextMenuEvent *event) {
    QListView::contextMenuEvent(event);
    if (this->library_model_proxy->rowCount() != 0) {
        this->context_menu->popup(event->globalPos());
    }
}

void LibraryView::receive_setMangaList_request(const QList<Manga> &manga_list) {
    this->library_model->setMangaList(manga_list);
    this->setCurrentIndex(this->library_model_proxy->getFirstIndex());
    this->updateLibraryViewStatus();
}

void LibraryView::receive_clearMangaList_request() {
    this->library_model->clearMangaList();
    this->current_item_filesize = QStringLiteral("File not found");
    this->updateLibraryViewStatus();
}

void LibraryView::receive_setSearchText_request(const QString &search_text) {
    this->library_model_proxy->setSearchText(search_text);
    this->updateLibraryViewStatus();

    if (Settings::select_first_item) {
        QModelIndex first_index = this->library_model_proxy->getFirstIndex();
        this->setCurrentIndex(first_index);
    }
}

void LibraryView::receive_selectRandomManga_request() {
    QModelIndex random_index = this->library_model_proxy->getRandomIndex();
    if (random_index.isValid()) {
        this->setCurrentIndex(random_index);
    }
}

void LibraryView::receive_showMangaInfoDialog_request() {
    this->showCurrentItemInfo();
}

void LibraryView::receive_scrollToCurrentItem_request() {
    QModelIndexList selected_idxs = this->selectedIndexes();
    if (selected_idxs.isEmpty()) {
        return;
    }

    this->scrollTo(selected_idxs.first(), QAbstractItemView::PositionAtCenter);
}

void LibraryView::libraryView_selectionModel_currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    // this prevents the filter model from selecting a new item
    // if the previous one got filtered out (is not visible)
    if (previous.isValid() && !Settings::select_first_item) {
        if (this->library_model_proxy->isIndexFiltered(previous)) {
            return;
        }
    }

    if (current.isValid()) {
        this->current_item_filesize = Utils::Fs::getFileSize(current.data(LibraryModel::FilePath).toString());
        this->updateLibraryViewStatus();
        emit send_LibraryView_currentChanged_path(current.data(LibraryModel::FilePath).toString());
    }
}

void LibraryView::show_info_action_triggered() {
    this->showCurrentItemInfo();
}

void LibraryView::copy_item_name_action_triggered() {
    QString items_name = this->getSelectedItemsData(LibraryModel::FileBasename);
    if (!Utils::Str::isNullOrEmpty(items_name)) {
        Clipboard::setText(items_name);
    }
}

void LibraryView::showCurrentItemInfo() {
    QModelIndexList selected_idxs = this->selectedIndexes();
    if (selected_idxs.isEmpty()) {
        return;
    }

    emit request_showMangaInfoDialog(Manga(
                selected_idxs.first().data(LibraryModel::FileHash).toString(),
                selected_idxs.first().data(LibraryModel::FilePath).toString(),
                selected_idxs.first().data(LibraryModel::FileBasename).toString(),
                selected_idxs.first().data(LibraryModel::Title).toString(),
                selected_idxs.first().data(LibraryModel::Artist).toStringList(),
                selected_idxs.first().data(LibraryModel::Parody).toStringList(),
                selected_idxs.first().data(LibraryModel::Circle).toStringList(),
                selected_idxs.first().data(LibraryModel::Magazine).toStringList(),
                selected_idxs.first().data(LibraryModel::Event).toStringList(),
                selected_idxs.first().data(LibraryModel::Publisher).toStringList(),
                selected_idxs.first().data(LibraryModel::Tags).toStringList()
                ));
}

void LibraryView::copy_title_action_triggered() {
    QString items_data = this->getSelectedItemsData(LibraryModel::Title);
    if (!Utils::Str::isNullOrEmpty(items_data)) {
        Clipboard::setText(items_data);
    }
}

void LibraryView::copy_path_action_triggered() {
    QString items_data = this->getSelectedItemsData(LibraryModel::FilePath);
    if (!Utils::Str::isNullOrEmpty(items_data)) {
        Clipboard::setText(items_data);
    }
}

void LibraryView::copy_hash_action_triggered() {
    QString items_data = this->getSelectedItemsData(LibraryModel::FileHash);
    if (!Utils::Str::isNullOrEmpty(items_data)) {
        Clipboard::setText(items_data);
    }
}

void LibraryView::copy_tags_action_triggered() {
    QString items_data = this->getSelectedItemsData(LibraryModel::Tags);
    if (!Utils::Str::isNullOrEmpty(items_data)) {
        Clipboard::setText(items_data);
    }
}

void LibraryView::remove_manga_action_triggered() {
    QStringList hash_list = QStringList();
    for (auto idx : this->selectedIndexes()) {
        hash_list.append(idx.data(LibraryModel::FileHash).toString());
    }
    if (!hash_list.isEmpty()) {
        emit send_LibraryView_deleteFromDatabase_request(hash_list);
    }
}

