/*
 * exception.hpp
 *
 *  Created on: 2012-9-6
 *      Author: YH
 */

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>
#include <string>

namespace matrix {
namespace base {

class Exception : public std::exception {
public:
    explicit Exception(const char* what);
    explicit Exception(const std::string& what);
    virtual ~Exception() noexcept;
public:
    virtual const char* what() const noexcept;
    virtual const std::string& message() const;
    const std::string& stackTrace() const;
private:
    void fillStackTrace();
private:
    std::string message_;
    std::string stack_;
};

}  /* namespace base */
}  /* namespace matrix */
#endif /* EXCEPTION_HPP_ */
