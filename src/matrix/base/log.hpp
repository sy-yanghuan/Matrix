/*
 * log.hpp
 *
 *  Created on: 2012-5-15
 *      Author: YH
 */
#ifndef LOG_HPP_
#define LOG_HPP_

#include <functional>
#include <vector>

#include <matrix/base/enum_stringize.hpp>
#include <matrix/base/string.hpp>
#include <matrix/base/singleton.hpp>

#define LOG_PTINTF(level, sign, ...) \
    base::Log::instance().printf(__FILE__, __LINE__, level, sign, __VA_ARGS__)

#define LOGT(sign,...) LOG_PTINTF(base::LogLevel::Trace, sign, __VA_ARGS__)
#define LOGD(sign,...) LOG_PTINTF(base::LogLevel::Debug, sign, __VA_ARGS__)
#define LOGI(sign,...) LOG_PTINTF(base::LogLevel::Info, sign, __VA_ARGS__)
#define LOGW(sign,...) LOG_PTINTF(base::LogLevel::Warning, sign, __VA_ARGS__)
#define LOGE(sign,...) LOG_PTINTF(base::LogLevel::Error, sign, __VA_ARGS__)
#define LOGF(sign,...) LOG_PTINTF(base::LogLevel::Fatal, sign, __VA_ARGS__)

namespace matrix {
namespace base {

const char kLogSignBase[] = "base";
const char kLogSignSystem[] = "system";

enum class LogLevel {
    Trace, Debug, Info, Warning, Error, Fatal
};

ENUM_TO_C_STR_FUNC_DEC(LogLevel);

class Log : public base::Singleton<Log> {
public:
    struct InfoPackage
    {
        const char* file;
        int line;
        LogLevel leve;
        const char* sign;
        const char* message;
    };
    typedef std::function<void(const InfoPackage&)> Callback;
public:
    void init();
    void printf(const char* file, int line, LogLevel leve, const char* sign, const char* format, ...);
    void print(const InfoPackage& package);
private:
    std::vector<Callback> v_;

friend class Singleton<Log> ;
};

}  // namespace base
}  // namespace matrix

#endif /* LOG_HPP_ */
