#pragma once

#include "app/librarymodel.hpp"
#include "app/librarymodelproxy.hpp"

#include <QListView>
#include <QWidget>
#include <QSortFilterProxyModel>


class LibraryView : public QListView {
    Q_OBJECT;
public:
    explicit LibraryView(QWidget *parent = nullptr);
    ~LibraryView() override;

private:
    LibraryModel *library_model;
    LibraryModelProxy *library_model_proxy;

    QString item_selected_filesize = nullptr;

    void updateLibraryViewStatus();

signals:
    void send_LibraryView_status(const QString &status);

public slots:
    void receive_setMangaList_request(QList<Manga> manga_list);
    void receive_clearMangaList_request();
    void receive_setSearchText_request(const QString &search_text);

private slots:
    void libraryView_selectionModel_currentChanged(const QModelIndex &current, const QModelIndex &previous);

};

