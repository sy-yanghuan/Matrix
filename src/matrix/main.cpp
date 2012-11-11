#include <matrix/base/convert.hpp>
#include <matrix/base/debug.hpp>
#include <matrix/base/log.hpp>
#include <matrix/platform/ptf_config.hpp>
#include <matrix/system/application.hpp>
#include <matrix/system/dispatcher_timer.hpp>
#include <matrix/system/renderEngine.hpp>

using namespace matrix;
using namespace matrix::system;

void handleEptr(std::exception_ptr eptr) {
    try {
        if (eptr != std::exception_ptr()) {
            std::rethrow_exception(eptr);
        }
    } catch (const std::exception& e) {
        std::string s = base::to<std::string>(e);
        LOGF(base::kLogSignSystem, s.c_str());
    }
}

int main(int argc, char* argv[]) {
    int code;
    try {
        Application app(argc, argv);
        BaseWindow& w = app.getBaseWindow();
        w.setTitleText(app.appPath());
        w.show();
        code = app.run();
    } catch (...) {
        std::exception_ptr e_ptr = std::current_exception();
        handleEptr(e_ptr);
        code = -1;
    }
    return code;
}

