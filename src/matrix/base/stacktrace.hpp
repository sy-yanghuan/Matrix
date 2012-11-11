/*
 * stacktrace.hpp
 *
 *  Created on: 2012-9-6
 *      Author: YH
 */

#ifndef STACKTRACE_HPP_
#define STACKTRACE_HPP_

#include <cstddef>

namespace matrix {
namespace base {

int stackTrace(void** result, int max_depth, int skip_count);
char* stacktrace(char* str, size_t size, const void* context);

}  /* namespace base */
}  /* namespace matrix */
#endif /* STACKTRACE_HPP_ */
