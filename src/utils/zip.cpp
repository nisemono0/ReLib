#include "utils/zip.hpp"
#include "utils/fs.hpp"
#include "utils/defs.hpp"

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


QJsonDocument Utils::Zip::getInfoJsonDocument(const QString &file_path) {
    QuaZipFile zip_file = QuaZipFile(file_path);
    zip_file.setFileName(ZipInfo::InfoJson, QuaZip::csInsensitive);

    // Will opne only if filename set with setFileName exists
    if (zip_file.open(QIODevice::ReadOnly)) {
        QByteArray json_byte_array = zip_file.readAll();
        zip_file.close();

        QJsonDocument json_doc = QJsonDocument::fromJson(json_byte_array);
        // If inside json.info is empty or it's not a json file return an empty QJsonObject
        if (json_doc.isNull() || !json_doc.isObject()) {
            return QJsonDocument();
        }

        return json_doc;
    }

    if (zip_file.isOpen()) {
        zip_file.close();
    }
    // Return emtpy QJsonDocument if info.json doesn't exist
    return QJsonDocument();
}

