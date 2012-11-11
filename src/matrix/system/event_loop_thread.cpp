/*
 * event_loop_thread.cpp
 *
 *  Created on: 2012-9-12
 *      Author: YH
 */

#include "event_loop_thread.hpp"

namespace matrix {
namespace system {

void MessageQueue::dequeue(std::vector<Message>* out) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this](){
        return !messages_.empty();
    });
    messages_.swap(*out);
}

void MessageQueue::enqueue(const Message& m) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        messages_.push_back(m);
    }
    condition_.notify_one();
}

EventLoopThread::EventLoopThread()
  : thread_(std::bind(&EventLoopThread::loop, this)) {
}

EventLoopThread::~EventLoopThread() {
    quit();
}

void EventLoopThread::quit() {
    quit_ = true;
    if(std::this_thread::get_id() != thread_.get_id()) {
        queue_.enqueue({});
        thread_.join();
    }
}

void EventLoopThread::loop() {
    std::vector<Message> v;
    while (!quit_) {
        queue_.dequeue(&v);
        for (Message& m : v) {
            if (m.callback) {
                m.callback();
            }else {
                if (onHandleMessage) {
                    onHandleMessage(m);
                }
            }
        }
        v.clear();
    }
}

}  // namespace system
}  // namespace matrix
