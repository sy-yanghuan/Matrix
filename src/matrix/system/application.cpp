/*
 * Application.cpp
 *
 *  Created on: 2012-5-9
 *      Author: YH
 */
#include "application.hpp"

#include <algorithm>

#include <matrix/base/log.hpp>
#include <matrix/platform/ptf_app.hpp>
#include <matrix/system/RenderEngine.hpp>

namespace matrix {
namespace system {

Application::Application(int argc,char* argv[])  {
    loadArguments(argc,argv);
    init();
}

Application::~Application() {

}

void Application::loadArguments(int argc,char* argv[]) {
    ASSERT(argc > 0);
    app_path_ = argv[0];
}

void Application::init() {
    app_impl_.init();
    window_.init();
    RenderEngine::instance();
}

void Application::exit(int exitCode) {
    app_impl_.exit(exitCode);
}

int Application::Application::run() {
    const int code = app_impl_.run();
    return code;
}

}  // namespace system
}  // namespace matrix

