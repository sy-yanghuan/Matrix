/*
 * string.hpp
 *
 *  Created on: 2012-5-10
 *      Author: YH
 */

#ifndef STRING_HPP_
#define STRING_HPP_

#include <string>
#include <type_traits>

#include <matrix/base/debug.hpp>
#include <matrix/base/range.hpp>

namespace matrix {
namespace base {

std::string stringPrintf(const char* format, ...);

void stringPrintf(std::string* output, const char* format, ...);

std::string& stringAppendf(std::string* output, const char* format, ...);

template<class CharIter>
class StringRange: public Range<CharIter> {
public:
    typedef Range<CharIter> SuperRange;
    typedef typename SuperRange::value_type value_type;
    typedef std::basic_string<value_type> string;
    typedef typename string::size_type size_type;
    typedef typename string::traits_type traits_type;
public:
    constexpr StringRange()
      : SuperRange() {}

    constexpr StringRange(CharIter begin,CharIter end)
      : SuperRange(begin,end) {}

    constexpr StringRange(CharIter begin, size_t size)
      : SuperRange(begin,begin + size) {}

    template<size_type N>
    constexpr StringRange(const value_type (&s)[N])
      : StringRange { s, s[N - 1] != '\0' ? N : N - 1 } {
    }

    //降低重载匹配优先级,使其先匹配数组类型
    template<class T,class = typename std::enable_if<std::is_same<T,value_type>::value>::type>
    StringRange(const T* s)
      : SuperRange(s,s + traits_type::length(s)) {}

    StringRange(const string& s)
      : SuperRange(s.data(),s.data() + s.size()) {}

    StringRange(const string& s, size_type start_from)
      : SuperRange(s.data() + start_from, s.data() + s.size()) {
        ASSERT(start_from < s.size());
    }

    StringRange(const string& s, size_type start_from, size_type size)
      : SuperRange(s.data() + start_from,s.data() + start_from + size) {
        ASSERT(start_from + size < s.size());
    }

public:
    using SuperRange::reset;

    const value_type* c_str() const {
        ASSERT(*this->end() == '\0');
        return data();
    }

    const value_type* data() const {
        return this->begin();
    }

    void reset(CharIter begin, size_type size) {
        this->reset(begin, begin + size);
    }

    void reset(const string& s) {
        this->reset(s.begin(), s.end());
    }

    string str() const {
        return string(this->begin(), this->size());
    }

    string toString() const {
        return str();
    }

    StringRange& operator =(const string& s) {
        this->reset(s);
        return *this;
    }
};

typedef StringRange<const char*> StringPiece;
typedef StringRange<const char16_t*> StringU16Piece;


enum class UtfConvResCode {
    Ok,
    SourceExhausted,
    TargetExhausted
};

UtfConvResCode convToUtf8(char16_t** source_start, const char16_t* source_end,
        char** target_start, const char* target_end);

UtfConvResCode convToUtf16(char** source_start, const char* source_end,
        char16_t** target_start,const char16_t* target_end);

std::string& stringUtf8Append(std::string* output, const StringU16Piece& u16str);

inline void stringUtf8To(std::string* output, const StringU16Piece& u16str) {
    output->clear();
    stringUtf8Append(output, u16str);
}

std::u16string& stringUtf16Append(std::u16string* output, const StringPiece& u8str);

inline void stringUtf16To(std::u16string* output, const StringPiece& u8str) {
    output->clear();
    stringUtf16Append(output, u8str);
}

std::string& demangleAppend(std::string* output,const char* name);

inline void demangleTo(std::string* output,const char* name) {
    output->clear();
    demangleAppend(output,name);
}

std::string& exceptionAppend(std::string* output,const std::exception& e);

inline void exceptionTo(std::string* output,const std::exception& e) {
    output->clear();
    exceptionAppend(output,e);
}

std::string& stackTraceAppend(std::string* s, const void* context);

} // namespace base
}  // namespace matrix

#endif /* STRING_HPP_ */
