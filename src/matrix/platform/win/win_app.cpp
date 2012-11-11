/*
 * win_app.cpp
 *
 *  Created on: 2012-9-11
 *      Author: YH
 */

#include "win_app.hpp"

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <d3d9.h>
#include <dinput.h>

#include <chrono>
#include <string>
#include <vector>

#include <matrix/base/convert.hpp>
#include <matrix/base/debug.hpp>
#include <matrix/base/exception.hpp>
#include <matrix/base/utility.hpp>
#include <matrix/base/log.hpp>
#include <matrix/platform/win/win_config.hpp>
#include <matrix/platform/win/win_utility.hpp>
#include <matrix/system/base_window.hpp>
#include <matrix/system/dispatcher_timer.hpp>
#include <matrix/system/keyboard_input.hpp>
#include <matrix/system/mouse_input.hpp>

using namespace std::chrono;

using namespace matrix;
using namespace matrix::system;

namespace matrix {
namespace platform {
namespace win {

const size_t kKeyboardCodeCount = 256;
const KeyCode kKeyCodeConvertTable[kKeyboardCodeCount] = {
        KeyCode::None,
        KeyCode::Escape,
        KeyCode::D1,
        KeyCode::D2,
        KeyCode::D3,
        KeyCode::D4,
        KeyCode::D5,
        KeyCode::D6,
        KeyCode::D7,
        KeyCode::D8,
        KeyCode::D9,
        KeyCode::D0,
        KeyCode::Minus,
        KeyCode::Equals,
        KeyCode::Back,
        KeyCode::Tab,
        KeyCode::Q,
        KeyCode::W,
        KeyCode::E,
        KeyCode::R,
        KeyCode::T,
        KeyCode::Y,
        KeyCode::U,
        KeyCode::I,
        KeyCode::O,
        KeyCode::P,
        KeyCode::LeftBracket,
        KeyCode::RightBracket,
        KeyCode::Enter,
        KeyCode::LeftCtrl,
        KeyCode::A,
        KeyCode::S,
        KeyCode::D,
        KeyCode::F,
        KeyCode::G,
        KeyCode::H,
        KeyCode::J,
        KeyCode::K,
        KeyCode::L,
        KeyCode::Semicolon,
        KeyCode::Apostrophe,
        KeyCode::AccentGrave,
        KeyCode::LeftShift,
        KeyCode::BackSlash,
        KeyCode::Z,
        KeyCode::X,
        KeyCode::C,
        KeyCode::V,
        KeyCode::B,
        KeyCode::N,
        KeyCode::M,
        KeyCode::Comma,
        KeyCode::Period,
        KeyCode::Slash,
        KeyCode::RightShift,
        KeyCode::NumPadMultiply,
        KeyCode::LeftAlt,
        KeyCode::Space,
        KeyCode::CapsLock,
        KeyCode::F1,
        KeyCode::F2,
        KeyCode::F3,
        KeyCode::F4,
        KeyCode::F5,
        KeyCode::F6,
        KeyCode::F7,
        KeyCode::F8,
        KeyCode::F9,
        KeyCode::F10,
        KeyCode::Unknown, //DIK_SCROLL
};

class AppState : public boost::noncopyable {
    static const size_t kKeyInputBufferSize = 16;
    static const size_t kMouseInputBufferSize = 32;
public:
    int exitCode = 0;
    HWND windowHwnd;
    _WinWindow* w = nullptr;

    AppState() {
        hInstance_ = GetModuleHandle(nullptr);
        ASSERT(app_state_ == nullptr);
        app_state_ = this;
    }
    ~AppState() {
        ASSERT(app_state_ != nullptr);
        app_state_ = nullptr;
    }
public:
    template<typename F>
    void addEventHandle(HANDLE handle, const F& f) {
        wait_event_handle_table_.push_back(handle);
        wait_event_func_table_.push_back(f);
    }

    HINSTANCE getAppHandle() {
        return hInstance_;
    }
    void initWindow();
    void initTimerEngine();
    void initInputDevices();
    int run();

    static AppState& instance() {
          ASSERT(app_state_ != nullptr);
          return *app_state_;
      }
private:
    void invokeEventFunc(size_t inx) {
        ASSERT(inx < wait_event_func_table_.size());
        std::function<void()>& f = wait_event_func_table_[inx];
        if (f) {
            f();
        }
    }
    KeyCode getKeyCode(uint32_t key_code) {
        ASSERT(key_code < base::sizeofArray(kKeyCodeConvertTable));
        return kKeyCodeConvertTable[key_code];
    }

    void fillMouseInputData(const DIDEVICEOBJECTDATA& data, MouseInputData* fill);

    void updateKeyInputState();
    void updateMouseInputState();
private:
    HINSTANCE hInstance_;
    std::vector<HANDLE> wait_event_handle_table_;
    std::vector<std::function<void()>> wait_event_func_table_;

    ComPtr<IDirectInput8A> direct_input_;
    ComPtr<IDirectInputDevice8A> keyboard_device_;
    ComPtr<IDirectInputDevice8A> mouse_device_;

    ComPtr<IDirect3D9> d3d_;
    ComPtr<IDirect3DDevice9> d3d_device_;

    std::array<uint8_t, kKeyboardCodeCount> key_input_diks_;
    DIMOUSESTATE2 mouse_input_diks_;
    static AppState* app_state_;
};

AppState* AppState::app_state_ = nullptr;

LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: {
            auto& func = AppState::instance().w->baseWindow.onCloseClickedFunc;
            if (!func || func()) {
                DestroyWindow(AppState::instance().windowHwnd);
            }
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(AppState::instance().exitCode);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void AppState::initWindow() {
    const wchar_t WINDOW_CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = { };

    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance_;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassW(&wc)) {
        throw std::runtime_error("RegisterClassW error!");
    }

    windowHwnd = CreateWindowExW(
            0, // Optional window styles.
            WINDOW_CLASS_NAME, // Window class
            L"dddd", //Window Title
            WS_OVERLAPPEDWINDOW, // Window style
            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, // Parent window
            NULL, // Menu
            hInstance_, // Instance handle
            NULL // Additional application data
            );
    if (!windowHwnd) {
        throw std::runtime_error("CreateWindowExW error!");
    }
}

void AppState::initTimerEngine() {
    const milliseconds interval(1);
    const milliseconds dueTime(0);

    //创建定时器,无属性,自动复位,匿名
    HANDLE handle = CreateWaitableTimer(NULL, FALSE, NULL);

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -(nanoseconds(dueTime).count() / 100);

    if (!SetWaitableTimer(handle, &liDueTime, interval.count(), NULL, NULL, FALSE)) {
        throw std::runtime_error("dispatcherTimerEngine init error!");
    }

    DispatcherTimerEngine& engine = DispatcherTimerEngine::instance();
    engine.setInterval(interval);
    engine.start(dueTime);

    //添加等待对象,以及触发事件
    addEventHandle(handle, std::bind(&DispatcherTimerEngine::update, &engine));
}

void AppState::initInputDevices() {
    //创建DirectInput8对象
    IDirectInput8A* direct_input;
    dx::throwIfFailed(
        DirectInput8Create(
            hInstance_,
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            (void **) &direct_input,
            nullptr)
    );
    direct_input_.reset(direct_input);


    //创建DirectInput8设备（键盘）
    IDirectInputDevice8A* keyboard_device;
    dx::throwIfFailed(
        direct_input_->CreateDevice(GUID_SysKeyboard, &keyboard_device, nullptr)
    );
    keyboard_device_.reset(keyboard_device);


    //为键盘设置格式
    dx::throwIfFailed(
        keyboard_device_->SetDataFormat(&c_dfDIKeyboard)
    );


    //为键盘设置行为
    dx::throwIfFailed(
        keyboard_device_->SetCooperativeLevel(windowHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)
    );

    //为键盘设置缓冲方式
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = kKeyInputBufferSize; // Arbitary buffer size

    dx::throwIfFailed(
        keyboard_device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)
    );

    //创建事件，为自动型（使用完自动置为无信号状态），初始化为无信号状态
    HANDLE handle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!handle) {
        throw std::runtime_error("CreateEvent keyboard error!");
    }

    //为键盘安装事件通知关联，并准备获取采集
    keyboard_device_->SetEventNotification(handle);
    keyboard_device_->Acquire();

    //添加等待对象,以及触发事件
    addEventHandle(handle, std::bind(&AppState::updateKeyInputState, this));

    //创建DirectInput8设备（鼠标）
    IDirectInputDevice8A* mouse_device;
    dx::throwIfFailed(
        direct_input_->CreateDevice(GUID_SysMouse, &mouse_device, NULL)
    );
    mouse_device_.reset(mouse_device);


    //为鼠标设置格式(8按键鼠标)
    dx::throwIfFailed(
        mouse_device_->SetDataFormat(&c_dfDIMouse2)
    );


    //为鼠标设置行为
    dx::throwIfFailed(
        mouse_device_->SetCooperativeLevel(windowHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)
    );


    //鼠标设置缓冲方式
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = kMouseInputBufferSize;

    dx::throwIfFailed(
        mouse_device_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)
    );

    handle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!handle) {
        throw std::runtime_error("CreateEvent mouse error!");
    }

    mouse_device_->SetEventNotification(handle);
    mouse_device_->Acquire();

    addEventHandle(handle, std::bind(&AppState::updateMouseInputState, this));

    /*
     _pD3D = Direct3DCreate9(D3D_SDK_VERSION);
     if(_pD3D == nullptr)
     {
     BOOST_THROW_EXCEPTION(std::runtime_error("Direct3DCreate9 error!"));
     }

     D3DPRESENT_PARAMETERS d3dpp;
     ZeroMemory( &d3dpp, sizeof( d3dpp ) );
     d3dpp.Windowed = TRUE; //不是全屏
     d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
     d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

     hr = _pD3D->CreateDevice(
     D3DADAPTER_DEFAULT,
     D3DDEVTYPE_HAL,
     WindowHwnd,
     D3DCREATE_SOFTWARE_VERTEXPROCESSING,
     &d3dpp,
     &_pd3dDevice);

     if (FAILED(hr)) {
     BOOST_THROW_EXCEPTION(std::runtime_error("CreateDevice IDirect3DDevice9 error!"));
     }

     IDirect3DSurface8* lpDSCursor;
     hr = _pd3dDevice->CreateImageSurface(32, 32, D3DFMT_A8R8G8B8,&lpDSCursor);
     */

}

void AppState::updateKeyInputState() {
    DIDEVICEOBJECTDATA didod[kKeyInputBufferSize]; // Receives buffered data
    KeyInputData data_buff[kKeyInputBufferSize];
    DWORD dwElements = kKeyInputBufferSize;

    dx::throwIfFailed(
        keyboard_device_->GetDeviceState(sizeof(key_input_diks_), key_input_diks_.data())
    );

    HRESULT hr = keyboard_device_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
    if (hr == DI_OK) {
        ASSERT(dwElements != 0);
        std::transform(didod, didod + dwElements, data_buff, [this](DIDEVICEOBJECTDATA& d) {
            return KeyInputData {getKeyCode(d.dwOfs) , d.dwData & 0x80};
        });
        KeyboardInput::instance().updateState( { data_buff, data_buff + dwElements });
    } else {
        LOGI(kLogSignWin,"GetDeviceData[%ld]", hr);
    }
}

void AppState::fillMouseInputData(const DIDEVICEOBJECTDATA& data, MouseInputData* fill) {
    auto f_btn = [](MouseInputData* fill_data,MouseCode code,DWORD dwData) {
        fill_data->code = code;
        fill_data->downState = dwData & 0x80;
    };

    auto f_wheel = [](MouseInputData* fill_data,MouseCode code,DWORD dwData) {
        fill_data->code = code;
        fill_data->increment = dwData;
    };

    switch (data.dwOfs) {
        case DIMOFS_BUTTON0: {
            f_btn(fill, MouseCode::LeftButton, data.dwData);
            break;
        }
        case DIMOFS_BUTTON1: {
            f_btn(fill, MouseCode::RightButton, data.dwData);
            break;
        }
        case DIMOFS_BUTTON2: {
            f_btn(fill, MouseCode::MidButton, data.dwData);
            break;
        }
        case DIMOFS_BUTTON3: {
            f_btn(fill, MouseCode::ExtraButton0, data.dwData);
            break;
        }
        case DIMOFS_BUTTON4: {
            f_btn(fill, MouseCode::ExtraButton1, data.dwData);
            break;
        }
        case DIMOFS_BUTTON5: {
            f_btn(fill, MouseCode::ExtraButton2, data.dwData);
            break;
        }
        case DIMOFS_BUTTON6: {
            f_btn(fill, MouseCode::ExtraButton3, data.dwData);
            break;
        }
        case DIMOFS_BUTTON7: {
            f_btn(fill, MouseCode::ExtraButton4, data.dwData);
            break;
        }
        case DIMOFS_X: {
            f_wheel(fill, MouseCode::MoveX, data.dwData);
            break;
        }
        case DIMOFS_Y: {
            f_wheel(fill, MouseCode::MoveY, data.dwData);
            break;
        }
        case DIMOFS_Z: {
            f_wheel(fill, MouseCode::Wheel, data.dwData);
            break;
        }
        default:
            ASSERT_MSG(false,"not the input type");
            break;
    }
}

void AppState::updateMouseInputState() {
    DIDEVICEOBJECTDATA didod[kMouseInputBufferSize]; // Receives buffered data
    MouseInputData data_buff[kMouseInputBufferSize];
    DWORD dwElements = kMouseInputBufferSize;

    dx::throwIfFailed(
        mouse_device_->GetDeviceState(sizeof(mouse_input_diks_), &mouse_input_diks_)
    );

    HRESULT hr = mouse_device_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
    if (hr == DI_OK) {
        ASSERT(dwElements != 0);
        std::transform(didod, didod + dwElements, data_buff, [this](DIDEVICEOBJECTDATA& d) {
            MouseInputData data;
            fillMouseInputData(d, &data);
            return data;
        });
        MouseInput::instance().updateState( { data_buff, data_buff + dwElements });
    } else {
        LOGI(kLogSignWin,"GetDeviceData[%ld]", hr);
    }
}

int AppState::run() {
    const size_t wait_count = wait_event_handle_table_.size();
    const HANDLE* wait_handle_table = wait_event_handle_table_.data();
    MSG msg = { };
    while (true) {
        size_t resoult_offset = MsgWaitForMultipleObjects(
                wait_count,
                wait_handle_table, FALSE,
                INFINITE,
                QS_ALLINPUT) - WAIT_OBJECT_0;

        if (resoult_offset < wait_count) {
            invokeEventFunc(resoult_offset);
        } else {
            while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                if (msg.message == WM_QUIT)
                    goto _Exit;

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    _Exit:
    return exitCode;
}

_WinApp::_WinApp()
  : app_state_(new AppState()) {
}

_WinApp::~_WinApp() {

}

int _WinApp::run() {
    return app_state_->run();
}

void _WinApp::exit(int exitCode) {
    app_state_->exitCode = exitCode;
    DestroyWindow(app_state_->windowHwnd);
}

void _WinApp::init() {
    app_state_->initWindow();
    app_state_->initTimerEngine();
    app_state_->initInputDevices();
}

_WinWindow::_WinWindow(system::BaseWindow& w)
  : baseWindow(w) {
}

void _WinWindow::init() {
    AppState::instance().w = this;
}

void _WinWindow::notifyTitleUpdate(const std::string& text) {
    std::u16string s = base::to<std::u16string>(text);
    SetWindowTextW(AppState::instance().windowHwnd, reinterpret_cast<const wchar_t*>(s.c_str()));
}

void _WinWindow::show() {
    ShowWindow(AppState::instance().windowHwnd, SW_SHOW);
    AppState::instance().w = this;
}


}  // namespace win
}  // namespace platform
}  // namespace matrix

typedef char* (*StackTrace)(char*, size_t, const void*);

static char* stacktraceEmpty(char* s, size_t, const void*) {
    return s;
}


namespace matrix {
namespace base {

extern StackTrace stacktrace_;

}  // namespace base
}  // namespace matrix

void loadBacktrace() {
    const char* st_moudel = "Backtrace.dll";
    const char* func_name = "stacktrace";

    HMODULE h = LoadLibraryA(st_moudel);
    if (h != nullptr) {
        base::stacktrace_ = reinterpret_cast<StackTrace>(GetProcAddress(h, func_name));
    }

    if(base::stacktrace_ == nullptr) {
        base::stacktrace_ = &stacktraceEmpty;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    LOGI(platform::win::kLogSignWin,"enter WinMain[%p]", &WinMain);

    loadBacktrace();

    if (AttachConsole(ATTACH_PARENT_PROCESS )) {
        if (_fileno(stdout) == -2) {
            freopen("CONOUT$","wb",stdout);
        }
        if (_fileno(stderr) == -2) {
            freopen("CONOUT$","wb",stderr);
        }
        if (_fileno(stdin) == -2) {
            freopen("CONIN$","rb",stdin);
        }
    }

    std::vector<std::string> args;
    {
        int num_args;
        LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &num_args);
        args.reserve(num_args);

        std::transform(arg_list, arg_list + num_args, std::back_inserter(args), [](LPWSTR arg) {
            const char16_t* c_str = reinterpret_cast<const char16_t*>(arg);
            return base::to<std::string>(c_str);
        });

        LocalFree(arg_list);
    }

    const int argc = args.size();
    char* argv[argc];

    std::transform(args.begin(), args.end(), argv, [](std::string& s) {
        return const_cast<char*>(s.c_str());
    });

    return main(argc, argv);
}



