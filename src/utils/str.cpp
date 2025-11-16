#include "utils/str.hpp"

#include <QString>


bool Utils::Str::isNullOrEmpty(const QString &str) {
    if (str.isNull() || str.isEmpty()) {
        return true;
    }
    return false;
}

