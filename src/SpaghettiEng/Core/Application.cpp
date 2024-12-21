#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

//external libs
//should be available because they were linked as public in Geom lib, which links to eng_lib
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Base.h"
#include "Log.h"
#include "Geom.h"
#include "Window.h"
#include "Application.h"

namespace Spg
{
  void Application::Initialise()
  {
    Log::Initialise();
    m_window = std::make_unique<Window>();
    m_window->Initialise();
  }

  void Application::Run()
  {
    m_window->RenderLoop();
  }

  void Application::Shutdown()
  {
    m_window->Shutdown();
  }

  void EngLibHello()
  {
    std::cout << "\nHello from Eng Lib\n";
    #ifdef SPG_DEBUG
      std::cout << "ENG: SPG_DEGUG DEFINED\n";
    #elif defined(SPG_RELEASE)
      std::cout << "ENG: SPG_RELEASE DEFINED\n";
    #endif

    #ifdef _WIN32
      std::cout << "ENG: WIN\n";
    #elif defined(__linux__)
      std::cout << "ENG: LINUX\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "ENG: x86_64\n";
    #endif

    glm::vec3 v{1.0,2.0,3.6};
   
    std::cout << "External libs linked into Eng Lib:\n";
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";
    std::cout << "STB_IMG: "  << STBI_VERSION << "\n";
    std::cout << "GLFW: " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "\n";
    std::cout << "IMGUI: " << IMGUI_VERSION << "\n";
    std::cout << "vec3 value is: " << v.x << "," << v.y << "," << v.z << "\n";
    std::cout << "\n";

    SPG_ASSERT(1);
    std::cout << "####################################################\n\n";
  }

}