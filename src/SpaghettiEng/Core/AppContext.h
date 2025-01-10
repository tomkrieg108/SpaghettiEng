#pragma once
#include <Common/SmartPointerUtils.h>
#include <Common/Logger.h>

#include "OpenGL32/GL32Renderer.h"
#include "Window.h" 

namespace Spg
{
  struct AppContext
  {
    Scope<Window> window = nullptr;
    Scope<GLRenderer> renderer = nullptr;
    std::string title{""};
    Utils::SpdLogger log;
    bool running = true;
  };
}