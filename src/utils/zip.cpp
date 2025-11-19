#include "utils/zip.hpp"
#include "utils/fs.hpp"
#include "utils/defs.hpp"

#include "base/log.hpp"

#include <quazip/JlCompress.h>
#include <quazip/quazipfile.h>

#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QVariantMap>
#include <QCollator>


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
        return QJsonDocument();
    }

    // json file inside our archive
    QuaZipFile zip_file = QuaZipFile(&zip_archive);
    // Try to open json file inside our archive for reading
    // Return empty json on error
    if (!zip_file.open(QIODevice::ReadOnly)) {
        zip_file.close();
        zip_archive.close();
        Log::error(QStringLiteral("[Utils::Zip::getInfoJsonDocument:zip_file.open]: %1").arg(file_path));
        return QJsonDocument();
    }

    // Read json QByteArray and close zip_file and zip_archive
    QByteArray json_byte_array = zip_file.readAll();
    zip_file.close();
    zip_archive.close();

    QJsonDocument json_doc = QJsonDocument::fromJson(json_byte_array);

    // If the json inside the archive is empty or if it's not a json file
    // return and empty QJsonDocument
    if (json_doc.isNull() || !json_doc.isObject()) {
        return QJsonDocument();
    }

    return json_doc;
}

