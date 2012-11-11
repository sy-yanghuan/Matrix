/*
 * ptf_id.hpp
 *
 *  Created on: 2012-9-11
 *      Author: YH
 */

#ifndef PTF_ID_HPP_
#define PTF_ID_HPP_

namespace matrix {
namespace platform {

enum class PlatformID {
    Unix, Linux, Windows, MacOSX, Xbox,
};

#if defined (__WINNT__)

constexpr PlatformID kPlatformID = PlatformID::Windows;

#elif defined(__linux__)

constexpr PlatformID kPlatformID = PlatformID::Linux;

#endif

constexpr PlatformID getPlatformID() {
    return kPlatformID;
}

}  // namespace platform
}  // namespace matrix

#endif /* PTF_ID_HPP_ */
