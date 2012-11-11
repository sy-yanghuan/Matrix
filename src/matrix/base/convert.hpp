/*
 * convert.hpp
 *
 *  Created on: 2012-6-24
 *      Author: YH
 */

#ifndef CONVERT_HPP_
#define CONVERT_HPP_

#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <boost/numeric/conversion/converter.hpp>

#include <matrix/base/enum_stringize.hpp>
#include <matrix/base/has_type/has_member_to_string.hpp>
#include <matrix/base/preprocessor.hpp>
#include <matrix/base/string.hpp>

namespace matrix {
namespace base {

/*******************************************************************************
 * Anything to string
 ******************************************************************************/

namespace detail {

#define CONVERT_RANGE_CHECK(condition, message)                           \
    ((condition) ? (void)0 : throw std::range_error(message))

template <class T>
struct IsSomeString {
    enum {
        value = std::is_same<T, std::string>::value
        || std::is_same<T, std::u16string>::value
        || std::is_same<T, StringPiece>::value
        || std::is_same<T, StringU16Piece>::value
    };
};

template<class Src>
typename std::enable_if<std::is_arithmetic<Src>::value, std::string&>::type
appendTo(std::string* s,const char* f, Src src) {
    const int kBufferSize =  std::numeric_limits<Src>::digits10
            + std::numeric_limits<Src>::max_exponent10  //指数
            + 1  //digits10的小数部分
            + 1  //'.' 小数点
            + 1  //'-' 负数
            + 1;  //'\0'
    char buffer[kBufferSize];
    const int count = snprintf(buffer, kBufferSize, f, src);
    ASSERT(count > 0 && count < kBufferSize);
    return s->append(buffer, count);
}
} // namespace detail

inline std::string& appendTo(std::string* s) {
    return *s;
}

inline std::string& appendTo(std::string* s, char c) {
    return *s += c;
}


inline std::string& appendTo(std::string* s, const StringPiece& src) {
    return s->append(src.data(), src.size());
}

inline std::string& appendTo(std::string* s, const StringU16Piece& src) {
    return stringUtf8Append(s, src);
}

inline std::string& appendTo(std::string* s, const std::exception& src) {
    return exceptionAppend(s,src);
}

inline std::string& appendTo(std::string* s, int src) {
    return detail::appendTo(s,"%d",src);
}

inline std::string& appendTo(std::string* s, unsigned int src) {
    return detail::appendTo(s,"%u",src);
}

inline std::string& appendTo(std::string* s, long src) {
    return detail::appendTo(s,"%ld",src);
}

inline std::string& appendTo(std::string* s, unsigned long src) {
    return detail::appendTo(s,"%lu",src);
}

inline std::string& appendTo(std::string* s, long long src) {
    return detail::appendTo(s,"%lld",src);
}

inline std::string& appendTo(std::string* s, unsigned long long src) {
    return detail::appendTo(s,"%llu",src);
}

inline std::string& appendTo(std::string* s, float src) {
    return detail::appendTo(s,"%f",src);
}

inline std::string& appendTo(std::string* s, double src) {
    return detail::appendTo(s,"%f",src);
}

//避免可以隐式转换的类型匹配
template<class Src>
typename std::enable_if<std::is_same<Src, bool>::value,std::string&>::type
appendTo(std::string* s, Src b) {
    return b ? s->append("true") : s->append("false");
}


template<class Src>
typename std::enable_if<std::is_convertible<Src, const char*>::value,std::string&>::type
appendTo(std::string* s,const Src& src) {
    const char* c_str = src;
    return c_str != nullptr ? s->append(c_str) : *s;
}

template<class Src>
typename std::enable_if<std::is_class<Src>::value
    && !detail::IsSomeString<Src>::value
    && !std::is_convertible<Src, std::exception>::value
    && !std::is_convertible<Src, const char*>::value,std::string&>::type
appendTo(std::string* s,const Src& src) {
    static_assert(has::has_member_named_toString<Src,std::string(Src::*)() const>::value,
            "member function 'std::string toString() const' not found");
    return *s += src.toString();
}

template<class Src>
typename std::enable_if<std::is_enum<Src>::value,std::string&>::type
appendTo(std::string* s,Src src) {
    const char* c_str = ENUM_TO_C_STR(src);
    return appendTo(s,c_str);
}

template<class Head, class ... Tail>
typename std::enable_if<sizeof...(Tail)>= 1, std::string&>::type
appendTo(std::string* s,const Head& head, const Tail&... tail) {
    appendTo(s, head);
    return appendTo(s, tail...);
}

template<class Tgt, class ... Src>
inline typename std::enable_if<std::is_same<Tgt, std::string>::value, Tgt>::type
to(const Src&... src) {
    std::string s;
    appendTo(&s, src...);
    return s;
}

template<class Tgt>
typename std::enable_if<std::is_same<Tgt, std::u16string>::value, Tgt>::type
to(const StringPiece& src) {
    std::u16string s;
    stringUtf16Append(&s, src);
    return s;
}

/*******************************************************************************
 * Arithmetic to arithmetic
 ******************************************************************************/
template <class Tgt, class Src>
typename std::enable_if<std::is_arithmetic<Src>::value && std::is_arithmetic<Tgt>::value,Tgt>::type
to(Src src) {
    typedef boost::numeric::converter<Tgt, Src> Converter;
    return Converter::convert(src);
}

/*******************************************************************************
 * String to Arithmetic
 ******************************************************************************/
namespace detail {

const size_t kStrToDigitsBuffSize = 120;

template<class Tgt, class F>
Tgt strToDigits(const F& f, const StringPiece& s) {
    char* end_ptr;
    const char* c_str = s.c_str();
    Tgt resoult = f(c_str, &end_ptr);
    if (end_ptr == c_str) {
        CONVERT_RANGE_CHECK(false, to<std::string>("string head not has digits :",s));
    } else {
        for (; *end_ptr != '\0'; ++end_ptr) {
            CONVERT_RANGE_CHECK(isblank(*end_ptr),
                    to<std::string>("string not only has digits :",s));
        }
    }
    return resoult;
}

template<class Tgt,class F>
Tgt strToDigits(const F& f,const StringPiece& s,int base) {
    auto f_ = std::bind(f,std::placeholders::_1,std::placeholders::_2,base);
    return strToDigits<Tgt>(f_, s);
}

bool strToBool(const StringPiece& src);

}  // namespace detail

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,int>::value
    || std::is_same<Tgt,long>::value,Tgt>::type
to(const StringPiece& src , int radix = 0) {
    return detail::strToDigits<Tgt>(&std::strtol, src, radix);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,unsigned int>::value
    || std::is_same<Tgt,unsigned long>::value,Tgt>::type
to(const StringPiece& src, int radix = 0) {
    return detail::strToDigits<Tgt>(&std::strtoul, src, radix);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,long long>::value,Tgt>::type
to(const StringPiece& src, int radix = 0) {
    return detail::strToDigits<Tgt>(&std::strtoll, src, radix);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,unsigned long long>::value,Tgt>::type
to(const StringPiece& src, int radix = 0) {
    return detail::strToDigits<Tgt>(&std::strtoull, src, radix);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,float>::value,Tgt>::type
to(const StringPiece& src) {
    return detail::strToDigits<Tgt>(&std::strtof, src);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,double>::value,Tgt>::type
to(const StringPiece& src) {
    return detail::strToDigits<Tgt>(&std::strtod, src);
}

template <class Tgt>
typename std::enable_if<std::is_same<Tgt,bool>::value,Tgt>::type
to(const StringPiece& src) {
    return detail::strToBool(src);
}

}  // namespace base
}  // namespace matrix



#endif /* CONVERT_HPP_ */
