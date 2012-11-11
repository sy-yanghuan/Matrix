/*
 * log.cpp
 *
 *  Created on: 2012-6-23
 *      Author: YH
 */
#include "log.hpp"

#include <cstdarg>
#include <cstdio>
#include <memory>

#include <matrix/base/convert.hpp>

namespace matrix {
namespace base {
namespace detail {

const int kfulshBuffSize = 4 * 1024;

}  // namespace detail

const char* kLogLevelName[] = {
        STRINGIZE(Trace),
        STRINGIZE(Debug),
        STRINGIZE(Info),
        STRINGIZE(Warning),
        STRINGIZE(Error),
        STRINGIZE(Fatal),
};

ENUM_TO_C_STR_FUNC_DEF(LogLevel,kLogLevelName);

void writeLogToStdout(const Log::InfoPackage& package) {
    std::printf(
            "[%s][%s][%s]--%s-%d\n",
            ENUM_TO_C_STR(package.leve),
            package.sign,
            package.message,
            package.file,
            package.line);
}

void Log::init()
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    v_.push_back(&writeLogToStdout);
}

void Log::printf(const char* file, int line, LogLevel leve, const char* sign, const char* format, ...) {
    char str_buff[detail::kfulshBuffSize];
    va_list args;
    va_start(args, format);
    int bytes_used = vsnprintf(str_buff, detail::kfulshBuffSize, format, args);
    if (bytes_used < 0) {
        throw std::runtime_error(to<std::string>(
                 "Invalid format string; snprintf returned negative "
                 "with format string: ", format));
    } else if (bytes_used < detail::kfulshBuffSize) {
        print({ file, line, leve, sign, str_buff });
    } else {
        const int kRefulshBuffSize = bytes_used + 1;
        std::unique_ptr<char[]> str_buff_ptr(new char[kRefulshBuffSize]);
        bytes_used = vsnprintf(str_buff_ptr.get(), kRefulshBuffSize, format, args);
        if (bytes_used + 1 != kRefulshBuffSize) {
            throw std::runtime_error(to<std::string>(
                            "vsnprint retry did not manage to work "
                            "with format string: ", format));
        }
        print({ file, line, leve, sign, str_buff_ptr.get() });
    }
    va_end(args);
}

void Log::print(const InfoPackage& package) {
    for (auto& i : v_) {
        i(package);
    }
}


}  // namespace base
}  // namespace matrix
