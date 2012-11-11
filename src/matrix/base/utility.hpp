/*
 * utility.hpp
 *
 *  Created on: 2012-8-8
 *      Author: YH
 */

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <cstddef>   //size_t

namespace matrix {
namespace base {

template<typename T,size_t N>
constexpr size_t sizeofArray(T (&)[N]) {
    return N;
}

template<typename Tp>
void safeDelete(Tp*& t) {
    if (t != nullptr) {
        static_assert(sizeof(Tp) > 0,"can't delete pointer to incomplete type");
        delete t;
        t = nullptr;
    }
}

template<typename Tp>
void safeDeleteArray(Tp*& t) {
    if (t != nullptr) {
        static_assert(sizeof(Tp) > 0,"can't delete pointer to incomplete type");
        delete[] t;
        t = nullptr;
    }
}

template<typename T>
void unused(const T&) {}

template<typename T, typename Member>
constexpr size_t offsetOf(Member T::* ptr_to_member) {
    return reinterpret_cast<size_t>(&(reinterpret_cast<T*>(0)->*ptr_to_member));
}

template<typename T, typename Member>
T* baseAddressOf(Member T::* ptr_to_member, Member* address_of_member) {
    return reinterpret_cast<T *>(reinterpret_cast<unsigned char*>(address_of_member) - offsetOf(ptr_to_member));
}

}  // namespace base
}  // namespace matrix


#endif /* UTILITY_HPP_ */
