#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

//external libs
//should be available because they were linked as public in Geom lib, which links to eng_lib
//only for void EngLibHello()
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

//only for ImGui::ShowDemoWindow();
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>



#include "Events/EventManager.h"
#include "ImGuiLayer/ImGuiLayer.h"

//#include "Base.h" //in Application.h
//#include "Log.h" //in Base.h
#include "Geom.h"
#include "Window.h"
#include "Application.h"

namespace Spg
{
  static void OnMousePress(EventMouseButtonPressed& e)
  {
    LOG_WARN("App: Mouse Pressed {},{}", e.x, e.y);
  }
  static void OnMouseRelease(EventMouseButtonReleased& e)
  {
   LOG_WARN("App: Mouse Released {},{}", e.x, e.y);
  }

  static void OnMouseMoved(EventMouseMoved& e)
  {
    LOG_WARN("App Mouse Moved: {},{}: ", e.delta_x, e.delta_y);
  }

  static void OnMouseScrolled(EventMouseScrolled& e)
  {
    LOG_WARN("App Mouse scrolled: {},{}: ", e.x_offset, e.y_offset);
  }

  static void OnWindowResized(EventWindowResize& e)
  {
    LOG_WARN("App Window FB resize: {},{}: ", e.buffer_height, e.buffer_height);
  }

  static void OnWindowClosed(EventWindowClose& e)
  {
    LOG_WARN("App Window closed");
  }

  Application::Application() 
  {
  }

  void Application::Initialise()
  {
    Log::Initialise();
    EventManager::Initialise(); 
    m_window = CreateScope<Window>();
    ImGuiLayer::Initialise(*m_window);
    SetEventHandlers();
  }

  void Application::SetEventHandlers()
  {
    EventManager::AddHandler(OnMousePress);
    EventManager::AddHandler(OnMouseRelease);
    EventManager::AddHandler(OnMouseMoved);
    EventManager::AddHandler(OnMouseScrolled);
    EventManager::AddHandler(OnWindowResized);
    EventManager::AddHandler(this, &Application::OnWindowClosed);
    EventManager::AddHandler(this, &Application::OnKeyPressed);
    //EventManager::AddHandler(OnWindowClosed);
  }

  void Application::OnWindowClosed(EventWindowClose& e)
  {
    LOG_WARN("App Window closed **");
    m_running = false;
    e.handled = true;   
  }
  void Application::OnKeyPressed(EventKeyPressed& e)
  {
    LOG_WARN("Escape pressed **");
    if(e.key == GLFW_KEY_ESCAPE)
    {
      m_running = false;
      e.handled = true;   
    }
  }

  void Application::Run()
  {
    //m_window->RenderLoop();
    LOG_INFO("App loop starting");
    while(m_running)
    {
      m_window->ClearBuffers();
      m_window->PollEvents();
      
      if(m_window->IsMinimised())
        continue;
      
      ImGuiLayer::PreRender();
      ImGui::ShowDemoWindow();
      ImGuiLayer::PostRender();
        
      m_window->SwapBuffers();

      EventManager::DispatchQueuedEvents();
    }
  }

  void Application::Shutdown()
  {
    ImGuiLayer::Shutdown();
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