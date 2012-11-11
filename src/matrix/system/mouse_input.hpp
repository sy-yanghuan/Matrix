/*
 * mouse_input.hpp
 *
 *  Created on: 2012-6-8
 *      Author: YH
 */

#ifndef MOUSE_INPUT_HPP_
#define MOUSE_INPUT_HPP_

#include <matrix/base/enum_stringize.hpp>
#include <matrix/base/singleton.hpp>
#include <matrix/base/range.hpp>

namespace matrix {
namespace system {

enum class MouseCode{
    LeftButton,
    MidButton,
    RightButton,
    ExtraButton0,
    ExtraButton1,
    ExtraButton2,
    ExtraButton3,
    ExtraButton4,
    MoveX,
    MoveY,
    Wheel,
};

ENUM_TO_C_STR_FUNC_DEC(MouseCode);

struct MouseInputData {
    MouseCode code;
    union{
        bool downState;
        int  increment;
    };
};

class MouseInput : public base::Singleton<MouseInput> {
public:
    void updateState(const base::Range<MouseInputData*>& range);
};

}  // namespace system
}  // namespace matrix


#endif /* MOUSE_INPUT_HPP_ */
