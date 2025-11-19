#include "utils/zip.hpp"
#include "utils/str.hpp"
#include "utils/defs.hpp"

#include "base/log.hpp"

#include <quazip/JlCompress.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QStringList>
#include <QJsonDocument>


QJsonDocument Utils::Zip::getInfoJsonDocument(const QString &file_path) {
    QuaZip zip_archive = QuaZip(file_path);

    // If zip archive can't be openend return empty json
    if (!zip_archive.open(QuaZip::mdUnzip)) {
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getInfoJsonDocument:zip_archive.open]: %1").arg(file_path));
        return QJsonDocument();
    }

    // Get ZipInfo::InfoJson file from archive
    QStringList info_json_file = zip_archive.getFileNameList().filter(ZipInfo::InfoJson, Qt::CaseInsensitive);
    // If no file is found, close the archive and return an empty json
    if (info_json_file.isEmpty()) {
        zip_archive.close();
        return QJsonDocument();
    }
    // Try and set current file inside archive to our json file
    // Return empty json on error
    if (!zip_archive.setCurrentFile(info_json_file[0])) {
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getInfoJsonDocument:zip_json.setCurrentFile]: %1").arg(file_path));
        return QJsonDocument();
    }

    // json file inside our archive
    QuaZipFile zip_json = QuaZipFile(&zip_archive);
    // Try to open json file inside our archive for reading
    // Return empty json on error
    if (!zip_json.open(QIODevice::ReadOnly)) {
        zip_json.close();
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getInfoJsonDocument:zip_json.open]: %1").arg(file_path));
        return QJsonDocument();
    }

    // Read json QByteArray and close zip_json and zip_archive
    QByteArray json_byte_array = zip_json.readAll();
    zip_json.close();
    zip_archive.close();

    QJsonDocument json_doc = QJsonDocument::fromJson(json_byte_array);

    // If the json inside the archive is empty or if it's not a json file
    // return and empty QJsonDocument
    if (json_doc.isNull() || !json_doc.isObject()) {
        return QJsonDocument();
    }

    return json_doc;
}

QByteArray Utils::Zip::getArchiveFirstImage(const QString &file_path) {
    QuaZip zip_archive = QuaZip(file_path);

    if (!zip_archive.open(QuaZip::mdUnzip)) {
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getArchiveFirstImage:zip_archive.open]: %1").arg(file_path));
        return QByteArray();
    }

    // Get archive files list and natural sort it
    QStringList archive_files_list = zip_archive.getFileNameList();
    Utils::Str::naturalSort(archive_files_list);

    // Get the first found image file name inside the archive
    QString image_file = Utils::Str::getFirstImage(archive_files_list);

    // If no image file is found return empty QByteArray
    if(Utils::Str::isNullOrEmpty(image_file)) {
        zip_archive.close();
        return QByteArray();
    }

    // Try and set the current file to our found image file
    if (!zip_archive.setCurrentFile(image_file)) {
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getArchiveFirstImage:zip_archive.open]: %1").arg(file_path));
        return QByteArray();
    }

    QuaZipFile zip_image = QuaZipFile(&zip_archive);
    // Try to open our image file for reading
    // Return empty QByteArray on error
    if (!zip_image.open(QIODevice::ReadOnly)) {
        zip_image.close();
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getArchiveFirstImage:zip_image.open]: %1").arg(file_path));
        return QByteArray();
    }

    QByteArray image_bytes = zip_image.readAll();
    zip_image.close();
    zip_archive.close();

    return image_bytes;
}

int Utils::Zip::getArchiveImageCount(const QString &file_path) {
    QStringList file_list = JlCompress::getFileList(file_path);
    Utils::Str::removeNonImages(file_list);
    return file_list.length();
}

