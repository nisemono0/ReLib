#include "app/librarymodel.hpp"


LibraryModel::LibraryModel(QObject *parent) : QAbstractListModel(parent) {
    this->manga_list = new QList<Manga>();
}

LibraryModel::~LibraryModel() {
    delete this->manga_list;
}


int LibraryModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return this->manga_list->size();
}

void LibraryModel::setMangaList(QList<Manga> &manga_list) {
    beginResetModel();

    this->manga_list->clear();
    for (auto it : manga_list) {
        this->manga_list->append(it);
    }

    endResetModel();
}

void LibraryModel::clearMangaList() {
    beginResetModel();

    this->manga_list->clear();

    endResetModel();
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const {
    const Manga manga = this->manga_list->at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return manga.file_basename;
        case LibraryModel::FileHash:
            return manga.file_hash;
        case LibraryModel::FilePath:
            return manga.file_path;
        case LibraryModel::FileBasename:
            return manga.file_basename;
        case LibraryModel::Title:
            return manga.title;
        case LibraryModel::Artist:
            return manga.artist;
        case LibraryModel::Parody:
            return manga.parody;
        case LibraryModel::Circle:
            return manga.circle;
        case LibraryModel::Magazine:
            return manga.magazine;
        case LibraryModel::Event:
            return manga.event;
        case LibraryModel::Publisher:
            return manga.publisher;
        case LibraryModel::Tags:
            return manga.tags;
        default:
            return QVariant();
            break;
    }
}

