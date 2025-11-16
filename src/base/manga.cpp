#include "base/manga.hpp"

Manga::Manga() {}

Manga::Manga(
        const QString &file_hash,
        const QString &file_path,
        const QString &file_basename,
        const QString &title,
        const QStringList &artist,
        const QStringList &parody,
        const QStringList &circle,
        const QStringList &magazine,
        const QStringList &event,
        const QStringList &publisher,
        const QStringList &tags
     ) :
    file_hash(file_hash),
    file_path(file_path),
    file_basename(file_basename),
    title(title),
    artist(artist),
    parody(parody),
    circle(circle),
    magazine(magazine),
    event(event),
    publisher(publisher),
    tags(tags) {}

bool Manga::operator==(const Manga &manga) const {
    if (
            this->file_hash == manga.file_hash &&
            this->file_path == manga.file_path
       ) {
        return true;
    }
    return false;
}

