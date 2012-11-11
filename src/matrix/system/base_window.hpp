/*
 * window.hpp
 *
 *  Created on: 2012-5-10
 *      Author: YH
 */

#ifndef BASE_WINDOW_HPP_
#define BASE_WINDOW_HPP_

#include <functional>
#include <memory>
#include <matrix/base/string.hpp>

#include <matrix/platform/ptf_app.hpp>

namespace matrix {
namespace system {

class BaseWindow : boost::noncopyable {
public:
    std::function<bool()> onCloseClickedFunc;
public:
    BaseWindow();
    ~BaseWindow();
public:
    void init();
    const std::string& getTitleText();
    void setTitleText(const base::StringPiece& text);
    void notifyTitleUpdate(const std::string& text);
    void show();
private:
    platform::WindowImpl window_impl_;
    std::string titleText_;
};

/**************************************************/
//inline functions area of class BaseWindow
/**************************************************/
inline const std::string& BaseWindow::getTitleText() {
    return titleText_;
}

inline void BaseWindow::setTitleText(const base::StringPiece& text) {
    titleText_.assign(text.begin(),text.end());
    notifyTitleUpdate(titleText_);
}

}  // namespace system
}  // namespace matrix

#endif /* BASE_WINDOW_HPP_ */
