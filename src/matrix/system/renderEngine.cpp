/*
 * RenderEngine.cpp
 *
 *  Created on: 2012-6-10
 *      Author: YH
 */

#include "RenderEngine.hpp"
#include <matrix/base/log.hpp>

using namespace std;

namespace matrix {
namespace system {

RenderEngine::RenderEngine() {
    using namespace std::placeholders;
    loop_.onHandleMessage = std::bind(&RenderEngine::handleMessage, this, _1);
}

void RenderEngine::renderFrame() {
}

void RenderEngine::handleMessage(const Message& m) {
    LOGI(base::kLogSignSystem, "HandleMessage[%d]", m.what);
}

}  // namespace system
}  // namespace matrix
