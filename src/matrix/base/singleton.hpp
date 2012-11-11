/*
 * singleton.hpp
 *
 *  Created on: 2012-6-7
 *      Author: YH
 */

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <mutex>

#include <boost/noncopyable.hpp>

#include <matrix/base/debug.hpp>
#include <matrix/base/at_exit.hpp>

namespace matrix {
namespace base {

template<typename T>
class Singleton : boost::noncopyable {
protected:
    typedef Singleton SingletonBase;
public:
    static T& instance() {
        std::call_once(once_flag_, &createInstance);
        return *instance_;
    }
protected:
    Singleton() {
#if !defined(NDEBUG)
        ASSERT(check_create_);
#endif
    }
    void init() {}
private:
    static void createInstance() {
        ASSERT(instance_ == nullptr);
#if !defined(NDEBUG)
        check_create_ = true;
#endif
        instance_ = new T();
        instance_->init();
        atexit(&releaseInstance);
    }
    static void releaseInstance() {
        ASSERT(instance_ != nullptr);
        delete instance_;
        instance_ = nullptr;
    }
private:
    static T* instance_;
    static std::once_flag once_flag_;
#if !defined(NDEBUG)
    static bool check_create_;
#endif
};

template<typename T>
T* Singleton<T>::instance_ = nullptr;

template<typename T>
std::once_flag Singleton<T>::once_flag_;

#if !defined(NDEBUG)
template<typename T>
bool Singleton<T>::check_create_ = false;
#endif

}  // namespace base
}  // namespace matrix

#endif /* SINGLETON_HPP_ */
