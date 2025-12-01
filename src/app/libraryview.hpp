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

    QString current_item_filesize = nullptr;

    QMenu *context_menu;
    QMenu *copy_context_menu;
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

    void showCurrentItemInfo();
    void loadCurrentItemImages();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void send_LibraryView_status(const QString &status);
    void send_LibraryView_deleteFromDatabase_request(const QStringList &hash_list);
    // Show the MangaInfoDialog for the current item
    void request_showMangaInfoDialog(const Manga &manga);
    // Sends the current selected item path to ImageView
    void send_LibraryView_currentChanged_path(const QString &file_path);

public slots:
    void receive_setMangaList_request(const QList<Manga> &manga_list);
    void receive_clearMangaList_request();
    void receive_setSearchText_request(const QString &search_text);
    void receive_selectRandomManga_request();

    void receive_showMangaInfoDialog_request();
    void receive_scrollToCurrentItem_request();

    void receive_selectNextItem_shortcut();
    void receive_selectPreviousItem_shortcut();

private slots:
    void libraryView_selectionModel_currentChanged(const QModelIndex &current, const QModelIndex &previous);

    // Context menu
    void show_info_action_triggered();
    void copy_item_name_action_triggered();
    void copy_title_action_triggered();
    void copy_path_action_triggered();
    void copy_hash_action_triggered();
    void copy_tags_action_triggered();
    void remove_manga_action_triggered();
};

