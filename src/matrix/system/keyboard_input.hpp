/*
 * keyboard_input.hpp
 *
 *  Created on: 2012-6-7
 *      Author: YH
 */

#ifndef KEYBOARD_INPUT_HPP_
#define KEYBOARD_INPUT_HPP_

#include <matrix/base/enum_stringize.hpp>
#include <matrix/base/range.hpp>
#include <matrix/base/singleton.hpp>

namespace matrix {
namespace system {

enum class KeyCode {
    None, // 一个指示没有键的特殊值

    Escape, // Esc（也称作 Escape）键
    F1, // F1 键
    F2, // F2 键
    F3, // F3 键
    F4, // F4 键
    F5, // F5 键
    F6, // F6 键
    F7, // F7 键
    F8, // F8 键
    F9, // F9 键
    F10, // F10 键
    F11, // F11 键
    F12, // F12 键
    Insert, // Insert 键
    Delete, // Del（也称作 Delete）键

    AccentGrave, // 开音符 键
    D1, // 1 键。
    D2, // 2 键。
    D3, // 3 键。
    D4, // 4 键。
    D5, // 5 键。
    D6, // 6 键。
    D7, // 7 键。
    D8, // 8 键。
    D9, // 9 键。
    D0, // 0（零）键。
    Minus,  //减号
    Equals, //等号
    Back, // Backspace 键。

    Tab, // Tab 键。
    Q,  //Q键
    W,  //W键
    E,  //E键
    R,  //R键
    T,  //T键
    Y,  //Y键
    U,  //U键
    I,  //I键
    O,  //O键盘
    P,  //P键
    LeftBracket,  // { 键
    RightBracket, // } 键
    BackSlash,       //   \ 键

    CapsLock, // Caps Lock 键。
    A, // A 键。
    S, // S 键。
    D, // D 键。
    F, // F 键。
    G, // G 键。
    H, // H 键。
    J, // J 键。
    K, // K 键。
    L, // L 键。
    Semicolon, // ; 键
    Apostrophe, //  , 键
    Enter, // Enter 键

    LeftShift,  // 左 shift 键
    Z, // Z 键
    X, // X 键
    C, // C 键
    V, // V 键
    B, // B 键
    N, // N 键
    M, // M 键
    Comma, // , 键
    Period,// . 键
    Slash, // / 键
    RightShift,  // 右 shift 键

    LeftCtrl, //左 Ctrl（控制）键。
    Win,      // win 键
    LeftAlt,  // 左 alt 键
    Space,    // 空格 键
    RightAlt,  // 右 alt 键
    RightControl,  // 右 Control 键
    Left, // 向左键
    Up, // 向上键
    Right, // 向右键
    Down, // 向下键

    Home, // home 键
    End, // end 键
    PageUp, // Page Up 键
    PageDown, // Page Down 键
    NumLock,  // Num Lock 键

    NumPadDivide, // /（除号）键
    NumPadMultiply, // *（乘号）键
    NumPadSubtract, // -（减号）键
    NumPadAdd, // +（加号）键
    NumPadEnter, // Enter 键
    NumPadDecimal, // . 键
    NumPad0, // 数字键盘上的 0 键。
    NumPad1, // 数字键盘上的 1 键。
    NumPad2, // 数字键盘上的 2 键。
    NumPad3, // 数字键盘上的 3 键。
    NumPad4, // 数字键盘上的 4 键。
    NumPad5, // 数字键盘上的 5 键。
    NumPad6, // 数字键盘上的 6 键。
    NumPad7, // 数字键盘上的 7 键。
    NumPad8, // 数字键盘上的 8 键。
    NumPad9, // 数字键盘上的 9 键。
    Unknown, // 一个指示键超出此枚举范围的特殊值
};

struct KeyInputData {
    KeyCode keyCode;
    bool downState;
};

ENUM_TO_C_STR_FUNC_DEC(KeyCode);

class KeyboardInput : public base::Singleton<KeyboardInput> {
public:
    void updateState(const base::Range<KeyInputData*>& range);
};

}  // namespace system
}  // namespace matrix


#endif /* KEYBOARD_INPUT_HPP_ */
