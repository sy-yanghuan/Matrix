/*
 * timer.hpp
 *
 *  Created on: 2012-5-22
 *      Author: YH
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <array>
#include <chrono>
#include <cstdint>
#include <type_traits>

#include <boost/noncopyable.hpp>

#include <matrix/base/debug.hpp>
#include <matrix/base/link_list.hpp>

namespace matrix {
namespace base {

namespace chro = std::chrono;
typedef chro::time_point<chro::high_resolution_clock, chro::milliseconds> TimePoint;

template<size_t OffsideArraySize>
class TimerEngine;

namespace detail {

class _Timer : boost::noncopyable {
public:
    virtual ~_Timer() {}
public:
    TimePoint getExpires() const {
        return expires_;
    }
    bool isEnabled() {
        return hook_.is_linked();
    }
    void stop() {
        hook_.unlink();
    }
protected:
    void setExpires(TimePoint expires) {
        expires_ = expires;
    }
    virtual void action() = 0;
private:
    LinkListHook hook_;
    TimePoint expires_;     //到期时间,从TimerEngine启动算起
protected:
    template<size_t OffsideArraySize>
    friend class base::TimerEngine;
};

const size_t k32OffsideArraySize = 4;

}  // namespace detail

template<typename T>
class BaseTimer: public detail::_Timer {
protected:
    typedef BaseTimer SuperTimer;

    void setExpires(chro::milliseconds offset_now) {
        detail::_Timer::setExpires(engineInstance().getLastTimePoint() + offset_now);
    }
    void insert() {
        engineInstance().insertTimer(this);
    }
private:
    T& engineInstance() {
        return T::engineInstance();
    }
};


template<size_t OffsideArraySize = detail::k32OffsideArraySize>
class TimerEngine {
    typedef detail::_Timer Timer;
    typedef LinkList<Timer, &Timer::hook_> List;

    template<int n>
    struct TimerAarry {
        enum {
            BITS = n, ArraySize = 1 << BITS, Mask = ArraySize - 1
        };
        int index = 0;
        std::array<List,ArraySize> array;
        //重载下表运算符方便操作
        List* operator[](size_t inx) {
            return &array[inx];
        }

        List* getCurrentInxList() {
            return operator [](index);
        }
    };

    enum {
        CareArrayBIOS = 8, OffsideArrayBIOS = 6,
    };

    typedef TimerAarry<CareArrayBIOS> CareArray;
    typedef TimerAarry<OffsideArrayBIOS> OffsideArray;
public:
    TimePoint getLastTimePoint() {
        return TimePoint(_interval * _timerJiffies);
    }
    void update() {
        this->updateTick();
    }
    void setInterval(chro::milliseconds interval) {
        _interval = interval;
    }
    void start(chro::milliseconds offset = 0) {
        _startTimestamp = chro::high_resolution_clock::now() + offset;
    }
private:
    template<int N>
    bool isInRange(int64_t inx) const {
        const int64_t i = 1LL;
        return inx < (i << (CareArray::BITS + N * OffsideArray::BITS));
    }

    template<int N>
    List* getList(int64_t expirse) {
        if (N == 0) {
            const size_t i = expirse & CareArray::Mask;
            return rootArray_[i];
        }
        const int i = N - 1;
        const size_t j = (expirse >> (CareArray::BITS + i * OffsideArray::BITS))
                & OffsideArray::Mask;
        return vecArray_[i][j];
    }

    template<size_t K>
    typename std::enable_if<(K > OffsideArraySize), List*>::type
    getLinkList(int64_t idx, int64_t expirse) {
        return nullptr;
    }

    template<size_t K>
    typename std::enable_if<K <= OffsideArraySize, List*>::type
    getLinkList(int64_t idx, int64_t expirse) {
        if (isInRange<K>(idx)) {
            return getList<K>(expirse);
        } else {
            return getLinkList<K + 1>(idx, expirse);
        }
    }
    List* getLinkList(int64_t idx, int64_t expirse) {
        return getLinkList<0>(idx, expirse);
    }

    void insertTimer(Timer* timer) {
        int64_t expirse = timer->expires_.time_since_epoch() / _interval;
        int64_t idx = expirse - _timerJiffies;
        List* list = getLinkList(idx, expirse);
        ASSERT_MSG(list!= nullptr, "out of range!");
        list->push_back(*timer);
    }

    int64_t getPastTimestampCount() {
        return (chro::high_resolution_clock::now() - _startTimestamp) / _interval - _timerJiffies;
    }

    void cascadeTimers(OffsideArray* array) {
        List* head = array->getCurrentInxList();
        head->clear_and_dispose([this](Timer* t) {
            this->insertTimer(t);
        });
        array->index = (array->index + 1) & OffsideArray::Mask; //(即加1取64的模)
    }

    void updateTick() {
        while (getPastTimestampCount() > 0) {
            if (!rootArray_.index) {
                size_t i = 0;
                do {
                    cascadeTimers(&vecArray_[i]);
                } while (vecArray_[i].index == 1 && ++i < OffsideArraySize);
            }

            List* list = rootArray_.getCurrentInxList();
            list->clear_and_dispose([](Timer* t) {
                t->action();
            });

            ++_timerJiffies;
            rootArray_.index = (rootArray_.index + 1) & CareArray::Mask; //(即加1取256的模)
        }
    }
private:
    CareArray rootArray_;
    std::array<OffsideArray,OffsideArraySize> vecArray_;
    chro::high_resolution_clock::time_point _startTimestamp;      //启动时的时间戳
    int64_t _timerJiffies = 0;       //开机以来运行的次数
    chro::milliseconds _interval;    //间隔

template<typename T>
friend class BaseTimer;
friend class detail::_Timer;
};

}  // namespace base
}  // namespace matrix

#endif /* TIMER_HPP_ */
