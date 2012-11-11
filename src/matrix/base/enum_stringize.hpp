/*
 * enum_stringize.hpp
 *
 *  Created on: 2012-8-4
 *      Author: YH
 */

#ifndef ENUM_STRINGIZE_HPP_
#define ENUM_STRINGIZE_HPP_

#include <matrix/base/debug.hpp>
#include <matrix/base/utility.hpp>

#define ENUM_TO_C_STR enumCstr
#define ENUM_TO_C_STR_FUNC_DEC(EnumName) const char* ENUM_TO_C_STR(EnumName)

#define ENUM_TO_C_STR_FUNC_DEF(EnumName,kArray) \
const char* ENUM_TO_C_STR(EnumName code) { \
    size_t inx = static_cast<size_t>(code); \
    ASSERT(inx < base::sizeofArray(kArray)); \
    return kArray[inx]; \
}struct _EnumToCtsrDef##EnumName##_

#endif /* ENUM_STRINGIZE_HPP_ */
