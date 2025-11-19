#pragma once

#include "utils/defs.hpp"


namespace Utils::Str {
    // Check if QString is null or empty
    bool isNullOrEmpty(const QString &str);
    // Natural sort a QStringList inplace
    void naturalSortQStringList(QStringList &string_list);
};

