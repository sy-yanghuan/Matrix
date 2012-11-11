/*
 * exception.cpp
 *
 *  Created on: 2012-9-6
 *      Author: YH
 */
#include "exception.hpp"

#include <matrix/base/string.hpp>

namespace matrix {
namespace base {

Exception::Exception(const char* what)
    : message_(what)
{
    fillStackTrace();
}

Exception::Exception(const std::string& what)
    : message_(what)
{
    fillStackTrace();
}

Exception::~Exception() noexcept {

}

const char* Exception::what() const noexcept {
    return message_.c_str();
}

const std::string& Exception::message() const {
    return message_;
}

const std::string& Exception::stackTrace() const {
    return stack_;
}

void Exception::fillStackTrace() {
    demangleAppend(&stack_, typeid(*this).name());
    stack_ += " : ";
    stack_ += message();
    stack_ += '\n';

    stackTraceAppend(&stack_, nullptr);
}

} /* namespace base */
} /* namespace matrix */
