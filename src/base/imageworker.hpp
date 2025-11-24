#pragma once

#include "utils/defs.hpp"

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QPixmap>


class ImageWorker : public QObject {
    Q_OBJECT;
public:
    explicit ImageWorker(QObject *parent = nullptr);
    ~ImageWorker();

private:
    // For use with QtConcurrent to remember file order
    struct ImageWorkerInfo {
        int image_number = -1;
        QPixmap pixmap = QPixmap();
    };
    QPixmap getPixmapFromData(const QByteArray &image_data);
    ImageWorkerInfo getPixmapFromData(const NumberImage &number_image);

signals:
    void send_ImageWorker_info(const QString &info);
    void send_ImageWorker_progress(int progress);
    void send_ImageWorker_data(const QMap<int, QPixmap> &data, int total_images);
    void send_ImageWorker_cover(const QPixmap &pixmap, int total_images);

public slots:
    void receive_getArchiveImages_request(const QString &file_path);
    void receive_getArchiveCover_request(const QString &file_path);

};

