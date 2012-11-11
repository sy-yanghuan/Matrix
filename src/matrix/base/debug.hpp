/*
 * debug.hpp
 *
 *  Created on: 2012-7-25
 *      Author: YH
 */

#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include <boost/assert.hpp>

#define ASSERT(expr)            BOOST_ASSERT(expr)
#define ASSERT_MSG(expr, msg)   ASSERT(expr && msg)

namespace matrix {
namespace base {

template<class F>
void debugInvoke(F& f) {
#if !defined(NDEBUG)
    f();
#endif
}

}  // namespace base
}  // namespace matrix


#endif /* DEBUG_HPP_ */
