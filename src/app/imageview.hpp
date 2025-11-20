#pragma once

#include "app/imagescene.hpp"

#include <QGraphicsView>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPointer>
#include <QProgressDialog>


class ImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

private:
    // TODO
    // Implement functions to load all images, next, prev, zoom etc.
    // Implement a fitScene method that fits the scene based on an enum from settings
    ImageScene *image_scene;

    ImageWorker *image_worker;
    QThread *image_thread;

    QPointer<QProgressDialog> imageview_progress_dialog;

    QMenu *context_menu;
    QAction *load_images_action;
    QAction *show_info_action;
    QAction *copy_image_action;
    QAction *jump_image_action;
    QAction *scroll_item_action;

    void initContextMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void send_ImageView_status(const QString &status);
    void request_getArchiveImages(const QString &file_path);
    void request_getArchiveCover(const QString &file_path);

    void request_LibraryView_showMangaInfoDialog();
    void request_LibraryView_loadCurrentItemImages();

public slots:
    void receive_LibraryView_currentChanged_path(const QString &file_path);
    void receive_LibraryVew_loadCurrentItemImages_path(const QString &file_path);
    void receive_ImageWorker_info(const QString &info);
    void receive_ImageWorker_progress(int progress);
    void receive_ImageWorker_data(const QMap<int, QGraphicsPixmapItem*> &data, int total_images);
    void receive_ImageWorker_cover(QGraphicsPixmapItem* pixmap_item, int total_images);

private slots:
    void load_images_action_triggered();
    void show_info_action_triggered();
    void copy_image_action_triggered();
    void jump_image_action_triggered();
    void scroll_item_action_triggered();

    void imageview_progress_dialog_canceled();

};

