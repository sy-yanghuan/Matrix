/*
 * invoked_when_exit.cpp
 *
 *  Created on: 2012-8-5
 *      Author: YH
 */

#include "at_exit.hpp"

#include <cstdlib> // atexit
#include <mutex>
#include <vector>

#include <matrix/base/utility.hpp>

namespace matrix {
namespace base {
namespace detail {

std::vector<std::function<void()>>* vec_ptr = nullptr;
std::mutex mutex_;

void destroy() {
    if (vec_ptr != nullptr) {
        for (auto i = vec_ptr->rbegin(); i != vec_ptr->rend(); ++i) {
            if (*i) {
                (*i)();
            }
        }
        safeDelete(vec_ptr);
    }
}

} // namespace detail

void atexit(const std::function<void()>& f) {
    std::lock_guard<std::mutex> lock(detail::mutex_);
    if (detail::vec_ptr == nullptr) {
        detail::vec_ptr = new std::vector<std::function<void()>>();
        std::atexit(&detail::destroy);
    }
    detail::vec_ptr->push_back(f);
}

}  // namespace base
}  // namespace matrix

