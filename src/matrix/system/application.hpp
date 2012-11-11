/*
 * Application.h
 *
 *  Created on: 2012-5-9
 *      Author: YH
 */
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <matrix/platform/ptf_app.hpp>
#include <matrix/system/base_window.hpp>

namespace matrix {
namespace system {

class Application {
public:
    Application(int argc,char* argv[]);
    ~Application();
public:
    void init();
    void exit(int exitCode = 0);
    const std::string& appPath();
    BaseWindow& getBaseWindow();
    int run();
private:
    void loadArguments(int argc,char* argv[]);
    void release();
private:
    platform::AppImpl app_impl_;
    BaseWindow window_;
    std::string app_path_;
};

/**************************************************/
//inline functions area of class
/**************************************************/
inline const std::string& Application::appPath() {
    return app_path_;
}
inline BaseWindow& Application::getBaseWindow()  {
    return window_;
}

}  // namespace system
}  // namespace matrix

#endif /* APPLICATION_H_ */
