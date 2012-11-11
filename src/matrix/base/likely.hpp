/*
 * likely.hpp
 *
 *  Created on: 2012-9-7
 *      Author: YH
 */

#ifndef LIKELY_HPP_
#define LIKELY_HPP_

#undef LIKELY
#undef UNLIKELY

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

#endif /* LIKELY_HPP_ */
