#pragma once

#include "app/imagescene.hpp"

#include <QGraphicsView>
#include <QWidget>

#include <QEvent>


class ImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

private:
    // TODO
    // Implement functions to load all images, next, prev, zoom etc.
    ImageScene *image_scene;

    ImageWorker *image_worker;
    QThread *image_thread;

signals:
    void send_ImageView_status(const QString &status);
    void request_getArchiveImages(const QString &file_path);
    void request_getArchiveCover(const QString &file_path);

public slots:
    // void receive_LibraryView_item_changed(const QString &file_path);
    void receive_ImageWorker_info(const QString &info);
    void receive_ImageWorker_progress(int progress);
    void receive_ImageWorker_data(const QMap<int, QGraphicsPixmapItem*> &data, int total_images);
    void receive_ImageWorker_cover(const QGraphicsPixmapItem* pixmap_item, int total_images);
};

