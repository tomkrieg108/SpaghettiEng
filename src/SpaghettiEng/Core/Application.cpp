
//#define SPG_LIB_LINK_CHECK
//#define SPG_PRINT_GRAPHICS_SPECS

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
#include "CoreLib/Logger.h"
#include "CoreLib/PlatformDetect/PlatformDetect.h"

#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/Core/AppLayer.h"
#include "SpaghettiEng/Resource/ResourceManager.h"
#include "SpaghettiEng/Render/Mesh/MeshCache.h"
#include "SpaghettiEng/Render/ShaderCache.h"
#include "SpaghettiEng/Scene/SceneManager.h"

#include "SpaghettiEng/ImGuiUtils/ImGuiUtils.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLContext.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer.h"

// {} []
namespace Spg
{
  namespace fs = std::filesystem;

  
  Application* Application::s_instance = nullptr;

  // Initialise static singletons - called in main before instantiating App
  void Application::SystemInit()
  {
    Core::Logger::Initialise();
   
    Spg::ResourceManager::Init();
    Spg::MeshCache::Init();

    //Spg::ShaderCache::Init();
  }

  Application::Application(const std::string& app_name) :
    m_app_name{app_name}
  {
    SPG_ASSERT(s_instance == nullptr);
    s_instance = this;

    SetAssetsPath();
    
    m_service_locator.Register<Window>(app_name);
    m_service_locator.Register<SceneManager>();
    
    Window& win = m_service_locator.Get<Window>();
    win.SetEventCallback( WinEvt::MakeCallback(this, &Application::OnWindowsEvent) );

    ImGuiUtils::Initialise(win);
    
    AppLayer* app_layer = new AppLayer(m_service_locator, "App Layer");
    m_layer_stack.PushOverlay(app_layer);

  #ifdef SPG_PRINT_GRAPHICS_SPECS
    win.GetGraphicsContext()->PrintSpecs();
  #endif

  #ifdef SPG_LIB_LINK_CHECK 
    PrintPlatformInfo();
    PrintExternalLibInfo();
  #endif
    
  }

  Application::~Application()
  {
    ImGuiUtils::Shutdown();
  }

  void Application::OnWindowsEvent(WinEvt::Event& event)
  {
    //Chain of responsibility (not an event queue)
    // WinEvt::Dispatcher d(event);
    // d.Dsipatch<WinEvt::WindowClose>([this](WinEvt::WindowClose& e) {OnWindowClosed(e);});
    // d.Dsipatch<WinEvt::MouseBtnPressed>([this](WinEvt::MouseBtnPressed& e) {OnMouseBtnPressed(e);});
    // d.Dsipatch<WinEvt::KeyPressed>([this](WinEvt::KeyPressed& e) {OnKeyPressed(e);});

    //This works fine - no need for the dispatcher in WindowsEvent.h!!
    switch(event.type)
    {
      case WinEvt::EventType::WindowClose: 
        OnWindowClosed(static_cast<WinEvt::WindowClose&>(event)); break;
      case WinEvt::EventType::KeyPressed:
        OnKeyPressed(static_cast<WinEvt::KeyPressed&>(event)); break; 
    } 

    for(auto itr = m_layer_stack.rbegin(); itr!=m_layer_stack.rend(); ++itr)
    {
      if(event.handled)
        break;
      (*itr)->OnEvent(event) ;
    }
  }

  void Application::OnWindowClosed(WinEvt::WindowClose& e)
  {
    SPG_WARN("App Window closed **");
    m_running = false;
    e.handled = true;   
  }

  void Application::OnKeyPressed(WinEvt::KeyPressed& e)
  {
    SPG_WARN("Key pressed ** {} ", e.key);
    if(e.key == GLFW_KEY_ESCAPE)
    {
      m_running = false;
      e.handled = true;   
    }
  }

  void Application::PushLayer(Layer* layer)
  {
    m_layer_stack.PushLayer(layer);
  }

  void Application::PopLayer(Layer* layer)
  {
    m_layer_stack.PopLayer(layer); 
  }

  
  void Application::Run()
  {
    SPG_WARN("App loop starting");
    auto delta_time = 0.0;
    auto last_time = glfwGetTime();

    Window& win = m_service_locator.Get<Window>();

    while(m_running)
    {
      //if(win.ShouldClose())
      // break;
        
      auto now = glfwGetTime(); //in seconds
      delta_time = now - last_time;
      last_time = now;

      win.Clear();
     
      if(!win.IsMinimised())
      {
        for (Layer* layer : m_layer_stack)
		      layer->Render(delta_time);  

        ImGuiUtils::PreRender();
        for (Layer* layer : m_layer_stack)
		      layer->ImGuiRender();
        ImGuiUtils::PostRender();
      }

      // This calls glfwPollEvents() - windows events are guaranteed to be triggered here, sycnhronously - there's no need to process input events inside the main loop or to queue input events
      win.OnUpdate();
    }
  }

  //=============================================================================
  // todo Move this to Assets modules
  //=============================================================================
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

  //=============================================================================
  // For checking external lib linkage
  //=============================================================================

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