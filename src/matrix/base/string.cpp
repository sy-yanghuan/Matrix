/*
 * utlity.cpp
 *
 *  Created on: 2012-5-13
 *      Author: YH
 */
#include "string.hpp"

#include <cxxabi.h>
#include <cstdarg>
#include <cstdlib>
#include <memory>
#include <typeinfo>

#include <matrix/base/convert.hpp>
#include <matrix/base/exception.hpp>
#include <matrix/base/stacktrace.hpp>
#include <matrix/base/utility.hpp>

extern "C" {
#include <matrix/extra/utf_cvt/cvtutf.h>
}

namespace matrix {
namespace base {
namespace detail {

void stringPrintfImpl(std::string& output, const char* format, va_list args) {
    const size_t write_point = output.size();
    size_t remaining = output.capacity() - write_point;
    output.resize(output.capacity());

    int bytes_used = vsnprintf(&output[write_point], remaining, format, args);
    if (bytes_used < 0) {
        throw std::runtime_error(to<std::string>(
                "Invalid format string; snprintf returned negative "
                "with format string: ", format));

    } else if (static_cast<size_t>(bytes_used) < remaining) {
        output.resize(write_point + bytes_used);
    } else {
        output.resize(write_point + bytes_used + 1);
        remaining = bytes_used + 1;
        bytes_used = vsnprintf(&output[write_point], remaining, format, args);
        if (static_cast<size_t>(bytes_used + 1) != remaining) {
            throw std::runtime_error(to<std::string>(
                    "vsnprint retry did not manage to work "
                    "with format string: ", format));
        }
        output.resize(write_point + bytes_used); //使最后一个字符变成'\0'
    }
}

const size_t kConvStrBuffSize = 4 * 1024;

} // namespace detail

std::string stringPrintf(const char* format, ...) {
    std::string ret(std::max(32ULL, strlen(format) * 2), '\0');
    ret.resize(0);
    va_list ap;
    va_start(ap, format);
    detail::stringPrintfImpl(ret, format, ap);
    va_end(ap);
    return ret;
}

void stringPrintf(std::string* output, const char* format, ...) {
    output->clear();
    va_list ap;
    va_start(ap, format);
    detail::stringPrintfImpl(*output, format, ap);
    va_end(ap);
}

std::string& stringAppendf(std::string* output, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    detail::stringPrintfImpl(*output, format, ap);
    va_end(ap);
    return *output;
}

UtfConvResCode convToUtf8(char16_t** source_start, const char16_t* source_end, char** target_start,
        const char* target_end) {
    const int result_code = NSConvertUTF16toUTF8(reinterpret_cast<unichar**>(source_start),
            reinterpret_cast<const unichar*>(source_end),
            reinterpret_cast<unsigned char**>(target_start),
            reinterpret_cast<const unsigned char*>(target_end));
    return static_cast<UtfConvResCode>(result_code);
}

UtfConvResCode convToUtf16(char** source_start, const char* source_end, char16_t** target_start,
        const char16_t* target_end) {
    const int result_code = NSConvertUTF8toUTF16(reinterpret_cast<unsigned char **>(source_start),
            reinterpret_cast<const unsigned char*>(source_end),
            reinterpret_cast<unichar**>(target_start),
            reinterpret_cast<const unichar*>(target_end));
    return static_cast<UtfConvResCode>(result_code);
}

std::string& stringUtf8Append(std::string* output, const StringU16Piece& u16str) {
    char str_buffer[detail::kConvStrBuffSize];
    char16_t* source_start = const_cast<char16_t*>(u16str.begin());
    const char16_t* source_end = u16str.end();
    char* target_start;
    char* target_end;
    std::unique_ptr<char[]> str_buffer_ptr;

    const size_t kBufferSize = u16str.size() * 4; //假设最多一个UTF16字符换算成4个UTF8字符
    if (kBufferSize < detail::kConvStrBuffSize) {
        target_start = str_buffer;
        target_end = target_start + detail::kConvStrBuffSize;
    } else {
        str_buffer_ptr.reset(new char[kBufferSize]);
        target_start = str_buffer_ptr.get();
        target_end = target_start + kBufferSize;
    }
    UtfConvResCode res = convToUtf8(&source_start, source_end, &target_start, target_end);
    ASSERT(res == UtfConvResCode::Ok);
    unused(res);
    return output->append(str_buffer, target_start);
}

std::u16string& stringUtf16Append(std::u16string* output, const StringPiece& u8str) {
    char16_t str_buffer[detail::kConvStrBuffSize];
    char* source_start = const_cast<char*>(u8str.begin());
    const char* source_end = u8str.end();
    char16_t* target_start;
    char16_t* target_end;
    std::unique_ptr<char16_t[]> str_buffer_ptr;

    const size_t kBufferSize = u8str.size() * 2;
    if (kBufferSize < detail::kConvStrBuffSize) {
        target_start = str_buffer;
        target_end = target_start + detail::kConvStrBuffSize;
    } else {
        str_buffer_ptr.reset(new char16_t[kBufferSize]);
        target_start = str_buffer_ptr.get();
        target_end = target_start + kBufferSize;
    }
    UtfConvResCode res = convToUtf16(&source_start, source_end, &target_start, target_end);
    ASSERT(res == UtfConvResCode::Ok);
    unused(res);
    return output->append(str_buffer, target_start);
}

std::string& demangleAppend(std::string* output, const char* name) {
    int status;
    std::unique_ptr<char, void (*)(void*)> real_name_ptr(
            abi::__cxa_demangle(name, nullptr, nullptr, &status),
            &std::free);
    if (status == 0) {
        const char* c_str = real_name_ptr.get();
        output->append(c_str);
    } else {
        output->append(name);
    }
    return *output;
}

std::string& exceptionAppend(std::string* output,const std::exception& e) {
    const Exception* e_ptr = dynamic_cast<const Exception*>(&e);
    if (e_ptr != nullptr) {
        output->append(e_ptr->stackTrace());
    } else {
        demangleAppend(output, typeid(e).name());
        output->append(e.what());
    }
    return *output;
}

std::string& stackTraceAppend(std::string* s, const void* context) {
    const size_t kStrBuffSize = 4 * 1024;
    std::unique_ptr<char[]> str_buff(new char[kStrBuffSize]);
    stacktrace(str_buff.get(), kStrBuffSize, context);
    return s->append(str_buff.get());
}

}  // namespace base
}  // namespace matrix

