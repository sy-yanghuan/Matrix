/*
 * win_utility.hpp
 *
 *  Created on: 2012-9-11
 *      Author: YH
 */

#ifndef WIN_UTILITY_HPP_
#define WIN_UTILITY_HPP_

#include <windows.h>

#include <matrix/base/convert.hpp>
#include <matrix/base/exception.hpp>

namespace matrix {
namespace platform {
namespace win {

const char* kLogSignWin = "win";

struct ComPtrRelease {
    void operator()(IUnknown* obj) {
        obj->Release();
    }
};

template<class T>
using ComPtr = std::unique_ptr<T, ComPtrRelease>;

namespace dx {

class DXException : base::Exception {
public:
    DXException(HRESULT hresult)
    : base::Exception(base::to<std::string>(hresult))
    {}
};

inline void throwIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw DXException(hr);
    }
}

}  // namespace dx


}  // namespace win
}  // namespace platform
}  // namespace matrix

#endif /* WIN_UTILITY_HPP_ */
