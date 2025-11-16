#include "utils/json.hpp"
#include "utils/str.hpp"
#include "utils/fs.hpp"
#include "utils/defs.hpp"

#include <quazip/JlCompress.h>
#include <quazip/quazipfile.h>

#include <QJsonDocument>
#include <QJsonArray>
#include <QVariantMap>


bool Utils::Json::isNullOrEmptyJsonDocument(const QJsonDocument &json_doc) {
    if (json_doc.isNull() || json_doc.isEmpty()) {
        return true;
    }
    return false;
}

bool Utils::Json::isInfoJsonFormat(const QJsonDocument &json_doc) {
    if (json_doc[QStringLiteral("gallery_info")].isNull() || json_doc[QStringLiteral("gallery_info")].isUndefined()) {
        return false;
    }
    return true;
}

Manga Utils::Json::convertJsonInfoToManga(const ZipData &zip_data) {
    Manga manga = Manga();

    QVariantMap json_root_vmap = zip_data.info_json[QStringLiteral("gallery_info")].toObject().toVariantMap();
    QVariantMap json_tags_vmap = zip_data.info_json[QStringLiteral("gallery_info")][QStringLiteral("tags")].toObject().toVariantMap();

    manga.file_hash = zip_data.file_hash;
    manga.file_path = zip_data.file_path;
    manga.file_basename = Utils::Fs::getCompleteBaseName(zip_data.file_path);
    if (Utils::Str::isNullOrEmpty(json_root_vmap[QStringLiteral("title")].toString())) {
        manga.title = manga.file_basename;
    } else {
        manga.title = json_root_vmap[QStringLiteral("title")].toString();
    }
    manga.artist = json_tags_vmap[QStringLiteral("artist")].toStringList();
    manga.parody = json_tags_vmap[QStringLiteral("parody")].toStringList();
    manga.circle = json_tags_vmap[QStringLiteral("circle")].toStringList();
    manga.magazine = json_tags_vmap[QStringLiteral("magazine")].toStringList();
    manga.event = json_tags_vmap[QStringLiteral("event")].toStringList();
    manga.publisher = json_tags_vmap[QStringLiteral("publisher")].toStringList();
    manga.tags = json_tags_vmap[QStringLiteral("other")].toStringList();

    return manga;
}

