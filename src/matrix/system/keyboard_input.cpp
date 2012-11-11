/*
 * keyboard_input.cpp
 *
 *  Created on: 2012-6-7
 *      Author: YH
 */

#include "keyboard_input.hpp"

#include <matrix/base/log.hpp>
#include <matrix/base/preprocessor.hpp>


namespace matrix {
namespace system {

const char* kKeyCodeName[] = {
        STRINGIZE(None),
        STRINGIZE(Escape),
        STRINGIZE(F1),
        STRINGIZE(F2),
        STRINGIZE(F3),
        STRINGIZE(F4),
        STRINGIZE(F5),
        STRINGIZE(F6),
        STRINGIZE(F7),
        STRINGIZE(F8),
        STRINGIZE(F9),
        STRINGIZE(F10),
        STRINGIZE(F11),
        STRINGIZE(F12),
        STRINGIZE(Insert),
        STRINGIZE(Delete),

        STRINGIZE(AccentGrave),
        STRINGIZE(D1),
        STRINGIZE(D2),
        STRINGIZE(D3),
        STRINGIZE(D4),
        STRINGIZE(D5),
        STRINGIZE(D6),
        STRINGIZE(D7),
        STRINGIZE(D8),
        STRINGIZE(D9),
        STRINGIZE(D0),
        STRINGIZE(Minus),
        STRINGIZE(Equals),
        STRINGIZE(Back),

        STRINGIZE(Tab),
        STRINGIZE(Q),
        STRINGIZE(W),
        STRINGIZE(E),
        STRINGIZE(R),
        STRINGIZE(T),
        STRINGIZE(Y),
        STRINGIZE(U),
        STRINGIZE(I),
        STRINGIZE(O),
        STRINGIZE(P),
        STRINGIZE(LeftBracket),
        STRINGIZE(RightBracket),
        STRINGIZE(BackSlash),

        STRINGIZE(CapsLock),
        STRINGIZE(A),
        STRINGIZE(S),
        STRINGIZE(D),
        STRINGIZE(F),
        STRINGIZE(G),
        STRINGIZE(H),
        STRINGIZE(J),
        STRINGIZE(K),
        STRINGIZE(L),
        STRINGIZE(Semicolon),
        STRINGIZE(Apostrophe),
        STRINGIZE(Enter),

        STRINGIZE(LeftShift),
        STRINGIZE(Z),
        STRINGIZE(X),
        STRINGIZE(C),
        STRINGIZE(V),
        STRINGIZE(B),
        STRINGIZE(N),
        STRINGIZE(M),
        STRINGIZE(Comma),
        STRINGIZE(Period),
        STRINGIZE(Slash),
        STRINGIZE(RightShift),

        STRINGIZE(LeftCtrl),
        STRINGIZE(Win),
        STRINGIZE(LeftAlt),
        STRINGIZE(Space),
        STRINGIZE(RightAlt),
        STRINGIZE(RightControl),
        STRINGIZE(Left),
        STRINGIZE(Up),
        STRINGIZE(Right),
        STRINGIZE(Down),

        STRINGIZE(Home),
        STRINGIZE(End),
        STRINGIZE(PageUp),
        STRINGIZE(PageDown),
        STRINGIZE(NumLock),

        STRINGIZE(NumPadDivide),
        STRINGIZE(NumPadMultiply),
        STRINGIZE(NumPadSubtract),
        STRINGIZE(NumPadAdd),
        STRINGIZE(NumPadEnter),
        STRINGIZE(NumPadDecimal),
        STRINGIZE(NumPad0),
        STRINGIZE(NumPad1),
        STRINGIZE(NumPad2),
        STRINGIZE(NumPad3),
        STRINGIZE(NumPad4),
        STRINGIZE(NumPad5),
        STRINGIZE(NumPad6),
        STRINGIZE(NumPad7),
        STRINGIZE(NumPad8),
        STRINGIZE(NumPad9),
        STRINGIZE(Unknown),
};

ENUM_TO_C_STR_FUNC_DEF(KeyCode,kKeyCodeName);

void KeyboardInput::updateState(const base::Range<KeyInputData*>& range) {
    for (const KeyInputData& i : range) {
        LOGD(base::kLogSignSystem, "%s:%d", ENUM_TO_C_STR(i.keyCode), i.downState);
    }
}

}  // namespace system
}  // namespace matrix
