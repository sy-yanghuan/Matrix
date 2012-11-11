/*
 * window.cpp
 *
 *  Created on: 2012-5-10
 *      Author: YH
 */

#include "base_window.hpp"
#include <matrix/platform/ptf_app.hpp>

namespace matrix {
namespace system {

BaseWindow::BaseWindow()
  : window_impl_(*this) {
}

BaseWindow::~BaseWindow() {
}

void BaseWindow::init() {
    window_impl_.init();
}

void BaseWindow::notifyTitleUpdate(const std::string& text) {
    window_impl_.notifyTitleUpdate(text);
}

void BaseWindow::show() {
    window_impl_.show();
}

}  // namespace system
}  // namespace matrix



