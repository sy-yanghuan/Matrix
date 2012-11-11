/*
 * dispatcher_timer.hpp
 *
 *  Created on: 2012-5-23
 *      Author: YH
 */

#ifndef DISPATCHER_TIMER_HPP_
#define DISPATCHER_TIMER_HPP_

#include <matrix/base/timer.hpp>
#include <matrix/base/singleton.hpp>

namespace matrix {
namespace system {

class DispatcherTimerEngine: public base::TimerEngine<>,public base::Singleton<DispatcherTimerEngine> {
public:
    static DispatcherTimerEngine& engineInstance() {
        return instance();
    }
};

class DispatcherTimer: public base::BaseTimer<DispatcherTimerEngine> {
public:
    typedef std::chrono::milliseconds milliseconds;

    std::function<void(intptr_t)> onTickFunc;
public:
    milliseconds getDuration() const {
        return duration_;
    }
    intptr_t getExtra() const {
        return extra_;
    }
    void setDueTime(milliseconds dueTime) {
        dueTime_ = dueTime;
    }
    void setDuration(milliseconds duration) {
        duration_ = duration;
    }
    void setExtra(intptr_t extra) {
        extra_ = extra;
    }
    void start() {
        setExpires(dueTime_);
        insert();
    }
    void stop() {
        SuperTimer::stop();
        duration_ = milliseconds::zero();
    }
private:
    void action() override;
private:
    milliseconds duration_;
    milliseconds dueTime_;
    intptr_t extra_;
};

}  // namespace system
}  // namespace matrix



#endif /* DISPATCHER_TIMER_HPP_ */
