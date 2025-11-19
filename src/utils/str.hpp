#pragma once

#include "utils/defs.hpp"


namespace Utils::Str {
    // Check if QString is null or empty
    bool isNullOrEmpty(const QString &str);
    // Natural sort a QStringList inplace
    void naturalSort(QStringList &string_list);
    // Remove non image types from a QStringList
    void removeNonImages(QStringList &string_list);
    // Return the first image type from a QStringList;
    QString getFirstImage(const QStringList &string_list);
};

