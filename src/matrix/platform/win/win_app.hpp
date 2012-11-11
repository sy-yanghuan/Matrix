/*
 * win_app.hpp
 *
 *  Created on: 2012-9-11
 *      Author: YH
 */

#ifndef WIN_APP_HPP_
#define WIN_APP_HPP_

#include <memory>

#include <boost/noncopyable.hpp>

namespace matrix {
namespace system {
class BaseWindow;
}  // namespace system

namespace platform {
namespace win {

class _WinApp : boost::noncopyable {
public:
    _WinApp();
    ~_WinApp();
public:
    void exit(int exitCode);
    void init();
    int run();
private:
    std::unique_ptr<class AppState> app_state_;
};

class _WinWindow : boost::noncopyable {
public:
    _WinWindow(system::BaseWindow& w);
public:
    void init();
public:
    void notifyTitleUpdate(const std::string& text);
    void show();
public:
    system::BaseWindow& baseWindow;
};

}  // namespace win

typedef win::_WinApp AppImpl;
typedef win::_WinWindow WindowImpl;

}  // namespace platform
}  // namespace matrix



#endif /* WIN_APP_HPP_ */
