#include "utils/str.hpp"

#include <QString>
#include <QCollator>
#include <QMutableListIterator>
#include <QMimeDatabase>
#include <QMimeType>


bool Utils::Str::isNullOrEmpty(const QString &str) {
    if (str.isNull() || str.isEmpty()) {
        return true;
    }
    return false;
}

void Utils::Str::naturalSort(QStringList &string_list) {
    QCollator natural_sort = QCollator();
    natural_sort.setNumericMode(true);
    natural_sort.setCaseSensitivity(Qt::CaseInsensitive);

    std::sort(string_list.begin(), string_list.end(), natural_sort);
}

void Utils::Str::removeNonImages(QStringList &string_list) {
    QMutableListIterator<QString> mutable_it = QMutableListIterator<QString>(string_list);

    QMimeDatabase mime_database = QMimeDatabase();
    QMimeType mime_type = QMimeType();
    while (mutable_it.hasNext()) {
        mime_type = mime_database.mimeTypeForFile(mutable_it.next());
        // If not valid mimetype or not an image, remove it from the list
        if (!mime_type.isValid() || !mime_type.name().startsWith(QStringLiteral("image/"), Qt::CaseInsensitive)) {
            mutable_it.remove();
        }
    }
}

QString Utils::Str::getFirstImage(const QStringList &string_list) {
    QMimeDatabase mime_database = QMimeDatabase();
    QMimeType mime_type = QMimeType();

    for (auto string_it : string_list) {
        mime_type = mime_database.mimeTypeForFile(string_it, QMimeDatabase::MatchExtension);
        if (mime_type.isValid() && mime_type.name().startsWith(QStringLiteral("image/"), Qt::CaseInsensitive)) {
            return string_it;
        }
    }

    return QString();
}

