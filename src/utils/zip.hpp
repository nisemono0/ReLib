#pragma once

#include <QString>
#include <QJsonObject>
#include <QVariantMap>


// Custom struct for zip files
struct ZipData {
    QString file_hash;
    QString file_path;
    QJsonDocument info_json;
};

namespace Utils::Zip {
    // Return a QJsonDocument with the archive's json.info or an empty one if none are found
    QJsonDocument getInfoJsonDocument(const QString &file_path);
};

