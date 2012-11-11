/*
 * dispatcher_timer.cpp
 *
 *  Created on: 2012-5-23
 *      Author: YH
 */

#include "dispatcher_timer.hpp"

namespace matrix {
namespace system {

void DispatcherTimer::action() {
    if (onTickFunc) {
        onTickFunc(extra_);
    }
    if (duration_ != milliseconds::zero()) {
        setExpires(duration_);
        insert();
    }
}

}  // namespace system
}  // namespace matrix

