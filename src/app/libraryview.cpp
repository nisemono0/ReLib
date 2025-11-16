#include "app/libraryview.hpp"

#include "utils/str.hpp"
#include "utils/fs.hpp"


LibraryView::LibraryView(QWidget *parent) : QListView(parent) {
    this->library_model = new LibraryModel(this);

    this->library_model_proxy = new LibraryModelProxy(this);
    this->library_model_proxy->setSourceModel(this->library_model);

    this->setModel(this->library_model_proxy);

    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, this, &LibraryView::libraryView_selectionModel_currentChanged);
}

LibraryView::~LibraryView() {
    delete this->library_model_proxy;
    delete this->library_model;
}

void LibraryView::updateLibraryViewStatus() {
    if (Utils::Str::isNullOrEmpty(this->item_selected_filesize)) {
        emit send_LibraryView_status(QStringLiteral("Showing: [%1/%2]").arg(
                    QString::number(this->library_model_proxy->rowCount()),
                    QString::number(this->library_model->rowCount())
                    ));
    } else {
        emit send_LibraryView_status(QStringLiteral("Showing: [%1/%2] | Filesize: [%3]").arg(
                    QString::number(this->library_model_proxy->rowCount()),
                    QString::number(this->library_model->rowCount()),
                    this->item_selected_filesize
                    ));
    }

}

void LibraryView::receive_setMangaList_request(QList<Manga> manga_list) {
    this->library_model->setMangaList(manga_list);
    this->updateLibraryViewStatus();
}

void LibraryView::receive_clearMangaList_request() {
    this->library_model->clearMangaList();
    this->item_selected_filesize = QStringLiteral("File not found");
    this->updateLibraryViewStatus();
}

void LibraryView::receive_setSearchText_request(const QString &search_text) {
    this->library_model_proxy->setSearchText(search_text);
    this->updateLibraryViewStatus();
    // TODO: When implemeting settings add a flag to auto select the first item found if it exists
    // if (this->library_model_proxy->rowCount() > 0 && this->select_after_search == true && !Utils::Str::isNullOrEmpty(search_text)){
    //     QModelIndex proxy_first_idx = this->library_model_proxy->index(0, 0);
    //     this->setCurrentIndex(proxy_first_idx);
    // }
}

void LibraryView::libraryView_selectionModel_currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);
    if (current.isValid()) {
        this->item_selected_filesize = Utils::Fs::getFileSize(current.data(LibraryModel::FilePath).toString());
        this->updateLibraryViewStatus();
    }
}

