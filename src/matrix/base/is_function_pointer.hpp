/*
 * is_member_function_pointer.hpp
 *
 *  Created on: 2012-8-7
 *      Author: YH
 */

#ifndef IS_FUNCTION_POINTER_HPP_
#define IS_FUNCTION_POINTER_HPP_

#include <type_traits>

namespace matrix {
namespace base {

template<class Tp>
struct is_function_pointer : public std::is_function<typename std::remove_pointer<Tp>::type>
{

};

}  // namespace base
}  // namespace matrix

#endif /* IS_FUNCTION_POINTER_HPP_ */
