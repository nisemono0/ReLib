#pragma once

#include "base/manga.hpp"

#include "utils/defs.hpp"
#include "utils/zip.hpp"

#include <QString>
#include <QJsonObject>
#include <QVariantMap>


namespace Utils::Json {
    // Check if json obj is empty
    bool isNullOrEmptyJsonDocument(const QJsonDocument &json_doc);
    // Check if QJsonObject is info.json formatted
    bool isInfoJsonFormat(const QJsonDocument &json_doc);
    // Return a Manga object for internal use from an info.json QJsonObject
    Manga convertJsonInfoToManga(const ZipData &zip_data);
}

