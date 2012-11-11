/*
 * at_exit.hpp
 *
 *  Created on: 2012-8-5
 *      Author: YH
 */

#ifndef AT_EXIT_HPP_
#define AT_EXIT_HPP_

#include <functional>

namespace matrix {
namespace base {

void atexit(const std::function<void()>& f);

}  // namespace base
}  // namespace matrix

#endif /* AT_EXIT_HPP_ */
