#include "utils/str.hpp"

#include <QString>
#include <QCollator>


bool Utils::Str::isNullOrEmpty(const QString &str) {
    if (str.isNull() || str.isEmpty()) {
        return true;
    }
    return false;
}

void Utils::Str::naturalSortQStringList(QStringList &string_list) {
    QCollator natural_sort = QCollator();
    natural_sort.setNumericMode(true);
    natural_sort.setCaseSensitivity(Qt::CaseInsensitive);

    std::sort(string_list.begin(), string_list.end(), natural_sort);
}

