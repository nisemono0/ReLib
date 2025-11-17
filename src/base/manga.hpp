#pragma once

#include <QString>
#include <QStringList>


// Stored in LibraryViewModel
class Manga {
public:
    QString file_hash;
    QString file_path;
    QString file_basename;
    QString title;
    QStringList artist;
    QStringList parody;
    QStringList circle;
    QStringList magazine;
    QStringList event;
    QStringList publisher;
    QStringList tags;

    Manga();
    Manga(
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
         );

    // operator== overload to check if 2 Manga obj ar the same
    bool operator==(const Manga &manga) const;
};

