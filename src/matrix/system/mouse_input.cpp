/*
 * mouse_input.cpp
 *
 *  Created on: 2012-6-8
 *      Author: YH
 */
#include "mouse_input.hpp"

#include <matrix/base/log.hpp>
#include <matrix/base/preprocessor.hpp>
#include <matrix/system/renderEngine.hpp>


namespace matrix {
namespace system {

const char* kMouseInputCodeName[] = {
        STRINGIZE(LeftButton),
        STRINGIZE(MidButton),
        STRINGIZE(RightButton),
        STRINGIZE(ExtraButton0),
        STRINGIZE(ExtraButton1),
        STRINGIZE(ExtraButton2),
        STRINGIZE(ExtraButton3),
        STRINGIZE(ExtraButton4),
        STRINGIZE(MoveX),
        STRINGIZE(MoveY),
        STRINGIZE(Wheel),
};

ENUM_TO_C_STR_FUNC_DEF(MouseCode,kMouseInputCodeName);

void MouseInput::updateState(const base::Range<MouseInputData*>& range) {
    for(const MouseInputData& i : range){
        LOGD(base::kLogSignSystem,"%s:%d",ENUM_TO_C_STR(i.code),i.increment);
      //  RenderEngine::instance().renderFrame();
    }
}

}  // namespace system
}  // namespace matrix
