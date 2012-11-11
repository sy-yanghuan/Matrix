/*
 * preprocessor.hpp
 *
 *  Created on: 2012-7-17
 *      Author: YH
 */

#ifndef PREPROCESSOR_HPP_
#define PREPROCESSOR_HPP_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#define STRING_CAT(a,b)         BOOST_PP_CAT(a,b)
#define STRINGIZE(text)         BOOST_PP_STRINGIZE(text)

namespace matrix {
namespace base {

}  // namespace base
}  // namespace matrix


#endif /* PREPROCESSOR_HPP_ */
