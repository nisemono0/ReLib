#pragma once

#include "app/librarymodel.hpp"
#include "app/librarymodelproxy.hpp"

#include <QListView>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QAction>


class LibraryView : public QListView {
    Q_OBJECT;
public:
    explicit LibraryView(QWidget *parent = nullptr);
    ~LibraryView() override;

private:
    LibraryModel *library_model;
    LibraryModelProxy *library_model_proxy;

    QString item_selected_filesize = nullptr;

    QMenu *context_menu;
    QAction *load_images_action; // TODO Implement after ImageView is done
    QAction *show_info_action;
    QAction *copy_item_name_action;
    QAction *copy_title_action;
    QAction *copy_path_action;
    QAction *copy_hash_action;
    QAction *copy_tags_action;
    QAction *remove_manga_action;

    void initContextMenu();
    void updateLibraryViewStatus();
    QString getSelectedItemsData(LibraryModel::LibraryRole role);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void send_LibraryView_status(const QString &status);
    void send_LibraryView_deleteFromDatabase_request(QStringList hash_list);
    void request_showMangaInfoDialog(const Manga &manga);

public slots:
    void receive_setMangaList_request(QList<Manga> manga_list);
    void receive_clearMangaList_request();
    void receive_setSearchText_request(const QString &search_text);


private slots:
    void libraryView_selectionModel_currentChanged(const QModelIndex &current, const QModelIndex &previous);

    // Context menu
    void load_images_action_triggered();
    void show_info_action_triggered();
    void copy_item_name_action_triggered();
    void copy_title_action_triggered();
    void copy_path_action_triggered();
    void copy_hash_action_triggered();
    void copy_tags_action_triggered();
    void remove_manga_action_triggered();
};

