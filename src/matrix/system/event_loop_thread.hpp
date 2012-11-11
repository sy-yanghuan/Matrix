/*
 * event_loop_thread.hpp
 *
 *  Created on: 2012-9-12
 *      Author: YH
 */

#ifndef EVENT_LOOP_THREAD_HPP_
#define EVENT_LOOP_THREAD_HPP_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <vector>
#include <thread>

#include <boost/noncopyable.hpp>

namespace matrix {
namespace system {

struct Message {
    typedef std::function<void()> Callback;

    int what = 0;
    intptr_t arg1 = 0;
    intptr_t arg2 = 0;
    Callback callback;
};

class MessageQueue : boost::noncopyable {
public:
    void dequeue(std::vector<Message>* out);
    void enqueue(const Message& m);
private:
    std::vector<Message> messages_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

class EventLoopThread : boost::noncopyable {
public:
    std::function<void(const Message&)> onHandleMessage;
public:
    EventLoopThread();
    ~EventLoopThread();
public:
    MessageQueue& queue();
private:
    void loop();
    void quit();
public:
    std::atomic_bool quit_ = ATOMIC_VAR_INIT(false);
    std::thread thread_;
    MessageQueue queue_;
};

/**************************************************/
//inline functions area of class EventLoopThread
/**************************************************/
inline MessageQueue& EventLoopThread::queue() {
    return queue_;
}

}    // namespace system
}    // namespace matrix
#endif /* EVENT_LOOP_THREAD_HPP_ */
