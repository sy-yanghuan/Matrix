/*
 * range.hpp
 *
 *  Created on: 2012-7-15
 *      Author: YH
 */

#ifndef RANGE_HPP_
#define RANGE_HPP_

#include <boost/range/iterator_range.hpp>

namespace matrix {
namespace base {

template<class Iter>
class Range : public boost::iterator_range<Iter>
{
    typedef boost::iterator_range<Iter> SuperIteratorRange;
    typedef std::reverse_iterator<Iter> reverse_iterator;
public:
    constexpr Range()
      : SuperIteratorRange() {}

    constexpr Range(Iter begin, Iter end)
      : SuperIteratorRange(begin, end) {}

    template<class R>
    Range(R& r)
      : SuperIteratorRange(r) {}

    template<class R>
    Range(const R& r)
      : SuperIteratorRange(r) {}
public:
    void clear() {
        *this = Range();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(SuperIteratorRange::end() - 1);
    }

    reverse_iterator rend() {
        return reverse_iterator(SuperIteratorRange::begin() - 1);
    }

    void reset(Iter begin, Iter end) {
        *this = Range(begin, end);
    }
};

template <class Iter>
inline Range<Iter> makeRange(Iter begin, Iter end) {
  return Range<Iter>(begin, end);
}

template <class R>
inline Range<typename boost::range_iterator<R>::type> makeRange(R& r) {
  return boost::make_iterator_range(r);
}

template <class R>
inline Range<typename boost::range_iterator<const R>::type> makeRange(const R& r) {
  return boost::make_iterator_range(r);
}

typedef Range<const unsigned char*> ByteRange;

#define STACK_VLA(name,T,N)  \
        T _vla_data_##name[N];  \
        RangeIterator<T> name(_vla_data_##name,_vla_data_##name + N)

}  // namespace base
}  // namespace matrix



#endif /* RANGE_HPP_ */
