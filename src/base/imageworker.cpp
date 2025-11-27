#include "base/imageworker.hpp"
#include "base/log.hpp"

#include "utils/defs.hpp"
#include "utils/zip.hpp"
#include "utils/str.hpp"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QByteArrayList>
#include <QThread>
#include <QtConcurrent>
#include <QtMinMax>


ImageWorker::ImageWorker(QObject *parent) : QObject(parent) {

}

ImageWorker::~ImageWorker() {

}

QPixmap ImageWorker::getPixmapFromData(const QByteArray &image_data) {
    QPixmap image_pixmap = QPixmap();

    if (!image_pixmap.loadFromData(image_data)) {
        return QPixmap();
    }

    return image_pixmap;
}

ImageWorker::ImageWorkerInfo ImageWorker::getPixmapFromData(const NumberImage &number_image) {
    ImageWorkerInfo worker_info;

    QPixmap image_pixmap = QPixmap();

    if (!image_pixmap.loadFromData(number_image.image_data)) {
        return worker_info;
    }

    worker_info.image_number = number_image.image_number;
    worker_info.pixmap = image_pixmap;

    return worker_info;
}

void ImageWorker::receive_getArchiveImages_request(const QString &file_path) {
    QuaZip zip_archive = QuaZip(file_path);
    // Try to open the archive in read mode
    if (!zip_archive.open(QuaZip::mdUnzip)) {
        zip_archive.close();
        Log::error(QStringLiteral("[ImageWorker:getAllImages:zip_archive.open]: %1").arg(file_path));
        emit send_ImageWorker_info("Not found");
        return;
    }

    // Get the archive file list then remove any non image files
    // and natural sort it
    QStringList archive_files_list = zip_archive.getFileNameList();
    Utils::Str::removeNonImages(archive_files_list);
    Utils::Str::naturalSort(archive_files_list);

    // Init zip_file to our archive
    QuaZipFile zip_file = QuaZipFile(&zip_archive);

    // Init our (image number, pixmap item) map
    QMap<int, QPixmap> pixmap_map = QMap<int, QPixmap>();

    qsizetype archive_files_len = archive_files_list.length();
    int progress = 0;

    for (int zip_file_idx = 0; zip_file_idx < archive_files_len; zip_file_idx += BatchSize::ImageLoad) {
        // On thread interruption
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_ImageWorker_progress(100);
            zip_file.close();
            zip_archive.close();
            // If no images are loaded send a message otherwise
            // send whatever images we managed to load
            if (pixmap_map.isEmpty()) {
                emit send_ImageWorker_info(QStringLiteral("No image"));
            } else {
                emit send_ImageWorker_data(pixmap_map);
            }
            return;
        }

        QList<NumberImage> batch_number_image = QList<NumberImage>();
        int current_batch_len = qMin(BatchSize::ImageLoad, archive_files_len - zip_file_idx);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            // Try to set the current zip_file to our image file
            // Stop everything on error
            if (!zip_archive.setCurrentFile(archive_files_list[zip_file_idx + batch_idx])) {
                zip_archive.close();
                Log::error(QStringLiteral("[ImageWorker:getAllImages:zip_archive.setCurrentFile]: %1").arg(file_path));
                emit send_ImageWorker_info("No image");
                return;
            }

            // Try to open the set zip_file inside our archive for reading
            // Stop everything on error
            if (!zip_file.open(QIODevice::ReadOnly)) {
                zip_file.close();
                zip_archive.close();
                Log::error(QStringLiteral("[ImageWorker:getAllImages:zip_file.open]: %1").arg(file_path));
                emit send_ImageWorker_info("No image");
                return;
            }

            NumberImage current_image;
            current_image.image_number = zip_file_idx + batch_idx;
            current_image.image_data = zip_file.readAll();
            // Close the current zip_file but don't close the whole archive
            zip_file.close();

            batch_number_image.append(current_image);
            progress = static_cast<int>((zip_file_idx + batch_idx + 1) * 100.0 / archive_files_len);
        }

        auto mapped_batch_results = QtConcurrent::blockingMapped(batch_number_image, [this](const NumberImage &number_image) {
                return this->getPixmapFromData(number_image);
                });

        for (auto batch_results_it : mapped_batch_results) {
            // If we couldnt load a pixmap, stop everything
            if (batch_results_it.pixmap.isNull()) {
                zip_archive.close();
                Log::error(QStringLiteral("[ImageWorker:getAllImages:batch_results_it.pixmap_item]: %1").arg(file_path));
                emit send_ImageWorker_info("No image");
                return;
            }
            pixmap_map[batch_results_it.image_number] = batch_results_it.pixmap;
        }

        emit send_ImageWorker_progress(progress);

    }

    emit send_ImageWorker_data(pixmap_map);

}

void ImageWorker::receive_getArchiveCover_request(const QString &file_path) {
    // Use the utility function since we dont need a progressbar for a single file
    QByteArray image_bytes = Utils::Zip::getArchiveFirstImage(file_path);

    if (image_bytes.isEmpty() || image_bytes.isNull()) {
        Log::error(QStringLiteral("[ImageWorker:getArchiveCover:image_bytes]: %1").arg(file_path));
        emit send_ImageWorker_info("No image");
        return;
    }

    QPixmap image_pixmap = this->getPixmapFromData(image_bytes);
    if (image_pixmap.isNull()) {
        Log::error(QStringLiteral("[ImageWorker:getArchiveCover:pixmap_item]: %1").arg(file_path));
        emit send_ImageWorker_info("No image");
        return;
    }

    int total_images = Utils::Zip::getArchiveImageCount(file_path);
    emit send_ImageWorker_cover(image_pixmap, total_images, file_path);
}

