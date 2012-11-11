/*
 * convert.cpp
 *
 *  Created on: 2012-6-24
 *      Author: YH
 */
#include "convert.hpp"

#include <matrix/base/utility.hpp>

namespace matrix {
namespace base {
namespace detail {

bool isBoolStrEq(const char* b, const char* e, const char* c_str, size_t len) {
    auto rb = e - 1, re = b;
    for (; rb != re; --rb) {
        if (!isspace(*rb)) {
            break;
        }
    }
    e = rb  + 1;
    return len == static_cast<size_t>(e - b) &&  std::equal(e, b , c_str, [](char l,char r) {
        return tolower(l) == r;
    });
}

bool strToBool(const StringPiece& src) {
    auto b = src.begin(), e = src.end();
    for (;; ++b) {
        CONVERT_RANGE_CHECK(b < e,
                        "No non-whitespace characters found in input string");
      if (!isspace(*b)) break;
    }

    bool result;
    switch (*b) {
        case '0':
        case '1': {
            StringPiece temp = { b , e };
            uint32_t value = to<uint32_t>(temp);
            CONVERT_RANGE_CHECK(value <= 1, "Integer overflow when parsing bool: must be 0 or 1");
            result = (value == 1);
            break;
        }
        case 't':
        case 'T': {
            const char kboolStr[] = "true";
            const bool is_equl = isBoolStrEq(b, e, kboolStr, sizeofArray(kboolStr));
            CONVERT_RANGE_CHECK(is_equl, "string can not convert to bool");
            result = true;
            break;
        }
        case 'f':
        case 'F': {
            const char kboolStr[] = "false";
            const bool is_equl = isBoolStrEq(b, e, kboolStr, sizeofArray(kboolStr));
            CONVERT_RANGE_CHECK(is_equl, "string can not convert to bool");
            result = false;
            break;
        }
        default:
            CONVERT_RANGE_CHECK(false, "string can not convert to bool");
            break;
    }
    return result;
}

}  // namespace detail
}  // namespace base
}  // namespace matrix

