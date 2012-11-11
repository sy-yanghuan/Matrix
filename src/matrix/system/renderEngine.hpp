/*
 * RenderEngine.hpp
 *
 *  Created on: 2012-6-10
 *      Author: YH
 */

#ifndef RENDERENGINE_HPP_
#define RENDERENGINE_HPP_

#include <chrono>

#include <matrix/base/singleton.hpp>
#include <matrix/system/event_loop_thread.hpp>

namespace matrix {
namespace system {

class RenderEngine: public base::Singleton<RenderEngine> {
    typedef std::chrono::high_resolution_clock::time_point TimePoint;
public:
    RenderEngine();
public:
    inline double getFPS() const;
    void renderFrame();
private:
    void handleMessage(const Message& m);
private:
    EventLoopThread loop_;
    TimePoint start_render_time_point_;
    uint64_t rendering_times_ = 0;
};

/**************************************************/
//inline functions area of class RenderEngine
/**************************************************/
double RenderEngine::getFPS() const{
    using namespace std::chrono;
    typedef std::ratio_divide<seconds::period, high_resolution_clock::duration::period> r;
    high_resolution_clock::duration dur = high_resolution_clock::now() - start_render_time_point_;
    const double durCount = static_cast<double>(dur.count() * r::den) / r::num;
    return rendering_times_ / durCount;
}

}  // namespace system
}  // namespace matrix

#endif /* RENDERENGINE_HPP_ */
