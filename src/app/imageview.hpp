#pragma once

#include "app/imagescene.hpp"

#include "base/imageworker.hpp"

#include <QGraphicsView>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPointer>
#include <QProgressDialog>
#include <QCursor>
#include <QMouseEvent>
#include <QInputDialog>


class ImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

private:
    // TODO
    // Implement more mouse event handlers (drag, zoom, etc.)
    ImageScene *image_scene;

    QString current_item_path;

    ImageWorker *image_worker;
    QThread *image_thread;

    QCursor cursor_next;
    QCursor cursor_previous;

    QPointer<QProgressDialog> imageview_progress_dialog;
    QPointer<QInputDialog> jump_image_dialog;

    QMenu *context_menu;
    QAction *load_images_action;
    QAction *show_info_action;
    QAction *copy_image_action;
    QAction *jump_image_action;
    QAction *scroll_item_action;

    void initContextMenu();
    void updateContextMenu(bool lock);

    void updateImageViewStatus(int current_image, int total_images, int image_width, int image_height, bool is_cover = false);
    void updateImageViewStatus(const QString &status);

    void scaleAndFitCurrentImage();
    void fitImage();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void send_ImageView_status(const QString &status);
    void request_getArchiveImages(const QString &file_path);
    void request_getArchiveCover(const QString &file_path);

    void request_LibraryView_showMangaInfoDialog();
    void request_LibraryView_scrollToCurrentItem();

public slots:
    void receive_LibraryView_currentChanged_path(const QString &file_path);
    void receive_ImageWorker_info(const QString &info);
    void receive_ImageWorker_progress(int progress);
    void receive_ImageWorker_data(const QMap<int, QPixmap> &data);
    void receive_ImageWorker_cover(const QPixmap &pixmap, int total_images, const QString &cover_file_path);
    void receive_clearImageView_request();
    void receive_scaleAndFitImage_request();

private slots:
    void load_images_action_triggered();
    void show_info_action_triggered();
    void copy_image_action_triggered();
    void jump_image_action_triggered();
    void scroll_item_action_triggered();

    void receive_ImageScene_info(int current_image, int total_images, int image_width, int image_height);

    void imageview_progress_dialog_canceled();

    void receive_fitImage_request();

};

