#pragma once

#include "utils/defs.hpp"

#include <QObject>
#include <QMap>
#include <QGraphicsPixmapItem>
#include <QByteArray>


class ImageWorker : public QObject {
    Q_OBJECT;
public:
    explicit ImageWorker(QObject *parent = nullptr);
    ~ImageWorker();

private:
    // For use with QtConcurrent to remember file order
    struct ImageWorkerInfo {
        int image_number = -1;
        QGraphicsPixmapItem* pixmap_item = nullptr;
    };
    QGraphicsPixmapItem* getPixmapItemFromData(const QByteArray &image_data);
    ImageWorkerInfo getPixmapItemFromData(const NumberImage &number_image);

signals:
    void send_ImageWorker_info(const QString &info);
    void send_ImageWorker_progress(int progress);
    void send_ImageWorker_data(const QMap<int, QGraphicsPixmapItem*> &data, int total_images);
    void send_ImageWorker_cover(QGraphicsPixmapItem* pixmap_item, int total_images);

public slots:
    void receive_getArchiveImages_request(const QString &file_path);
    void receive_getArchiveCover_request(const QString &file_path);

};

