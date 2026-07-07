//Paired header
#include "SpaghettiEng/Core/Application.h"

//C System headers

//Std lib headers
#include <filesystem>

//3rd party external libs
#include <glad/gl.h>    //need to include before <GLFW/glfw3.h>
#include <GLFW/glfw3.h> //for glfwGetTime(), GLFW_KEY_ESCAPE

//============================================================
//external libs - for testing out only
//should be available because they were linked as public in Geom/Spg lib and propagated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#ifdef _WIN32
  #include "ft2build.h"
  #include <freetype/freetype.h>
#endif
#include <entt/entity/registry.hpp>
//============================================================

//Project headers
#include "SpaghettiEng/Events/EventManager.h"
#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLContext.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer.h"
#include "Window.h"
#include "ServiceLocator.h"
#include "AppLayer.h"

#include "CoreLib/Logger.h"
#include "CoreLib/PlatformDetect/PlatformDetect.h"


namespace Spg
{
  namespace fs = std::filesystem;

  static void OnMousePress(EventMouseButtonPressed& e)
  {
    SPG_WARN("App: Mouse Pressed: {},{}", e.x, e.y);
  }
  static void OnMouseRelease(EventMouseButtonReleased& e)
  {
   SPG_WARN("App: Mouse Released: {},{}", e.x, e.y);
  }

  static void OnMouseMoved(EventMouseMoved& e)
  {
    SPG_WARN("App Mouse Moved: [{},{}], [{},{}] ", e.x, e.y, e.delta_x, e.delta_y);
  }

  static void OnMouseScrolled(EventMouseScrolled& e)
  {
    SPG_WARN("App Mouse scrolled: {},{} ", e.x_offset, e.y_offset);
  }

  static void OnWindowResized(EventWindowResize& e)
  {
    SPG_WARN("App Window FB resize: {},{} ", e.buffer_width, e.buffer_height);
  }

  static void OnWindowClosed(EventWindowClose& e)
  {
    SPG_WARN("App Window closed");
  }

  static void OnKeyReleased(EventKeyReleased& e)
  {
    SPG_WARN("App Key released: {} ", e.key);
  }
 
  Application* Application::s_instance = nullptr;

  void Application::SystemInit()
  {
    //Static method - called in main() before app is created
    Core::Logger::Initialise();
    EventManager::Initialise(); //todo - does nothing!
  }

  Application::Application(const std::string& app_name) :
    m_app_name{app_name}
  {
    SPG_ASSERT(s_instance == nullptr);
    s_instance = this;
    SetEventHandlers();
    SetAssetsPath();
  
    m_service_locator.Register<Window>(app_name);

    const Window& win = m_service_locator.Get<Window>();
    ImGuiUtils::Initialise(win);

    AppLayer* app_layer = new AppLayer(m_service_locator, "App Layer");
    m_layer_stack.PushOverlay(app_layer);

  #ifdef SPG_DEBUG
    GLContext::PrintVideoModes();
    GLContext::PrintGLParams();
    GLContext::PrintImplInfo();
  #endif  
    //For testing
    //SPG_ASSERT(1);
  }

  Application::~Application()
  {
  #ifdef SPG_DEBUG
    SPG_INFO("Max event queue size: {}", EventManager::GetMaxQueueSize());
    SPG_INFO("Current event queue size: {}", EventManager::GetCurrentQueueSize());
  #endif  
    ImGuiUtils::Shutdown();

    //Todo detatch / delete all layers
  }

  Application* Application::Instance()
  {
    SPG_ASSERT(s_instance != nullptr);
    return s_instance;
  }

  void Application::PushLayer(Layer* layer)
  {
    m_layer_stack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PopLayer(Layer* layer)
  {
    layer->OnDetach();
    m_layer_stack.PopLayer(layer); 
  }

  void Application::SetEventHandlers()
  {
  #ifdef SPG_CALLBACK_CHECK
    EventManager::AddHandler(OnMousePress);
    EventManager::AddHandler(OnMouseRelease);
    EventManager::AddHandler(OnMouseMoved);
    EventManager::AddHandler(OnMouseScrolled);
    EventManager::AddHandler(OnWindowResized);
    EventManager::AddHandler(OnKeyReleased);
  #endif
    EventManager::AddHandler(this, &Application::OnWindowClosed);
    EventManager::AddHandler(this, &Application::OnKeyPressed);
  }

  //Todo - this should be setup in SystemInit()
  void Application::SetAssetsPath()
  {
    //Todo - this will need to be changed.  Currently depends on the location of this source file, which won't work when app is 'deployed'
    fs::path this_file = fs::absolute(fs::path{__FILE__});
    fs::path assets_path = this_file.parent_path() / fs::path{"../../../Assets"};
    assets_path = fs::absolute(assets_path);
    if (!fs::exists(assets_path)) {
        SPG_ERROR("Assets path does not exist: {}", assets_path.string());
        return;
    }
    if (!fs::is_directory(assets_path)) {
        SPG_ERROR("Assets path is not a directory: {}", assets_path.string());
        return;
    }
    try {
        fs::current_path(assets_path);
    } catch (const fs::filesystem_error& e) {
        SPG_ERROR("Exception setting CWD to assets path. Msg: {} Error: {}", assets_path.string(), e.what());
        return;
    }
    SPG_INFO("Current working directory successfully set to: {}", fs::current_path().string());
  }

  void Application::OnWindowClosed(EventWindowClose& e)
  {
    SPG_WARN("App Window closed **");
    m_running = false;
    e.handled = true;   
  }

  void Application::OnKeyPressed(EventKeyPressed& e)
  {
    SPG_WARN("Key pressed ** {} ", e.key);
    if(e.key == GLFW_KEY_ESCAPE)
    {
      m_running = false;
      e.handled = true;   
    }
  }

  void Application::Run()
  {
    SPG_WARN("App loop starting");
    auto delta_time = 0.0;
    auto last_time = glfwGetTime();

    //Window& win = m_service_locator.Get<Window>("Window");
    Window& win = m_service_locator.Get<Window>();

    while(m_running)
    {
      auto now = glfwGetTime(); //in seconds
      delta_time = now - last_time;
      last_time = now;

      win.Clear();
     
      if(!win.IsMinimised())
      {
        for (Layer* layer : m_layer_stack)
		      layer->OnUpdate(delta_time);

        ImGuiUtils::PreRender();
        for (Layer* layer : m_layer_stack)
		      layer->OnImGuiRender();
        ImGuiUtils::PostRender();
      }

      win.OnUpdate();

      EventManager::DispatchQueuedEvents();
    }
  }

  void Application::PrintPlatformInfo()
  {
    SPG_INFO("Platform and Build:");
    SPG_TRACE("Architecture: {} (via ArchDetech.h) ", ARCH_NAME)
    SPG_TRACE("Compiler: {} (via CompilerDetech.h) ", COMPILER_NAME)
    SPG_TRACE("OS: {} (via OSDetech.h) ", OS_NAME)
    #if defined(SPG_DEBUG)
      SPG_TRACE("SPG_DEBUG defined (via cmake)");
    #elif defined(SPG_RELEASE)
        SPG_TRACE("SPG_RELEASE defined (via cmake)");
    #endif
  }

  void Application::PrintExternalLibInfo()
  {
    // #define VALUE 99
    // std::cout << STRINGIFY(VALUE);
    // std::cout << STRINGIFY_IMPL(VALUE);

    //auto logger = Core::Logger::Create("UTILS");

    SPG_WARN("External libs linked into UTILS Lib");
    SPG_TRACE("JSON: {}.{}.{}", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH );
    SPG_TRACE("FMT: {} ", FMT_VERSION);
    SPG_TRACE("CXXOPTS: {}.{}.{}",CXXOPTS__VERSION_MAJOR,CXXOPTS__VERSION_MAJOR, CXXOPTS__VERSION_PATCH);
    SPG_TRACE("SPDLOG: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
    SPG_TRACE("Entt {},{},{}", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR, ENTT_VERSION_PATCH );
    
    fs::path current_path = fs::current_path();
    fs::path source_file_path(__FILE__);
    SPG_TRACE("Current working directory is: {}", current_path.string());
    SPG_TRACE("This file is: {}", source_file_path.string());

    SPG_WARN( "####################################################");
  }
}