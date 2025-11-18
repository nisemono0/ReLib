#pragma once

#include <QString>
#include <QJsonObject>
#include <QVariantMap>


namespace Utils::Zip {
    // Return a QJsonDocument with the archive's json.info or an empty one if none are found
    QJsonDocument getInfoJsonDocument(const QString &file_path);
};

