/*
 * has_xxx.hpp
 *
 *  Created on: 2012-8-5
 *      Author: YH
 */
#ifndef HAS_XXX_HPP_
#define HAS_XXX_HPP_

#include <type_traits>

#include <matrix/base/is_function_pointer.hpp>

namespace matrix {
namespace base {
namespace has_type {
struct FunctionMember;
struct ObjectMember;
struct StaticFunctionMember;
struct AnyMember;
}  // namespace has
}  // namespace base
}  // namespace has_type

namespace has = matrix::base::has_type;

#define HAS_MEMBER_DEC(member_name) \
template<typename T, typename MemberType = has::AnyMember, typename = std::true_type> \
struct has_member_named_##member_name : std::false_type {}; \
\
template<typename T, typename MemberType> \
struct has_member_named_##member_name<T, \
        MemberType, \
        typename std::conditional< \
        std::is_same<MemberType, has::FunctionMember>::value, \
        std::integral_constant<bool, std::is_member_function_pointer<decltype(&T::member_name)>::value>, \
        typename std::conditional< \
            std::is_same<MemberType, has::ObjectMember>::value, \
            std::integral_constant<bool, std::is_member_object_pointer<decltype(&T::member_name)>::value>, \
            typename std::conditional< \
                std::is_same<MemberType, has::StaticFunctionMember>::value, \
                std::integral_constant<bool, base::is_function_pointer<decltype(&T::member_name)>::value>, \
                std::integral_constant<bool, std::is_same<decltype(&T::member_name),MemberType>::value> \
                >::type \
            >::type \
        >::type \
> : std::true_type {}; \
\
template<typename T> \
struct has_member_named_##member_name<T, \
        has::AnyMember, \
        std::integral_constant<bool, \
            std::is_member_function_pointer<decltype(&T::member_name)>::value \
            || std::is_member_object_pointer<decltype(&T::member_name)>::value \
            || base::is_function_pointer<decltype(&T::member_name)>::value \
        > \
> : std::true_type {}


#endif /* HAS_XXX_HPP_ */
