#include "utils/str.hpp"


bool Utils::String::isNullOrEmpty(const QString &str) {
    if (str.isNull() || str.isEmpty()) {
        return true;
    }
    return false;
}

