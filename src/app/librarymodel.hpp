#pragma once

#include "base/manga.hpp"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>


class LibraryModel : public QAbstractListModel {
    Q_OBJECT;
public:
    enum LibraryRole {
        FileHash     = Qt::UserRole + 1,
        FilePath     = Qt::UserRole + 2,
        FileBasename = Qt::UserRole + 3,
        Title        = Qt::UserRole + 4,
        Artist       = Qt::UserRole + 5,
        Parody       = Qt::UserRole + 6,
        Circle       = Qt::UserRole + 7,
        Magazine     = Qt::UserRole + 8,
        Event        = Qt::UserRole + 9,
        Publisher    = Qt::UserRole + 10,
        Tags         = Qt::UserRole + 11,
    };

    explicit LibraryModel(QObject *parent = nullptr);
    ~LibraryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setMangaList(const QList<Manga> &manga_list);
    void clearMangaList();

private:
    QList<Manga> *manga_list;

signals:

public slots:
};

