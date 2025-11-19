#pragma once

#include <QString>
#include <QJsonObject>
#include <QByteArray>


namespace Utils::Zip {
    // Return a QJsonDocument with the archive's json.info or an empty one if none are found
    QJsonDocument getInfoJsonDocument(const QString &file_path);
    // Return a QByteArray of the first image in the archive
    QByteArray getArchiveFirstImage(const QString &file_path);
};

