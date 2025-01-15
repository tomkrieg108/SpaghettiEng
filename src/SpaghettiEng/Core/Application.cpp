
#include <filesystem>

#include <glad/gl.h>    //need to include before <GLFW/glfw3.h>
#include <GLFW/glfw3.h> //for glfwGetTime(), GLFW_KEY_ESCAPE

#include "Events/EventManager.h"
#include "ImGuiUtils/ImGuiUtils.h"
#include "OpenGL32/GL32Context.h"
#include "OpenGL32/GL32Shader.h"
#include "OpenGL32/GL32Renderer.h"
#include "Window.h"
#include "AppContext.h"
#include "AppLayer.h"
#include "Application.h"

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
    //Call this in main() before app is created
    Utils::Logger::Initialise();
    EventManager::Initialise(); //todo - does nothing!
  }

  Application::Application(const std::string& app_name) :
    m_app_name{app_name}
  {
    SPG_ASSERT(s_instance == nullptr);
    s_instance = this;
    SetEventHandlers();
    SetAssetsPath();
  
    m_app_context.Set("Window", CreateRef<Window>(app_name) );
    m_app_context.Set("GLRenderer", CreateRef<GLRenderer>() );

    const Window& win = m_app_context.Get<Window>("Window");
    ImGuiUtils::Initialise(win);

    AppLayer* app_layer = new AppLayer(m_app_context, "App Layer");
    m_layer_stack.PushOverlay(app_layer);

    //Test shader
    GLShaderBuilder shader_builder;
    auto pbr_shader = shader_builder.Add(ShaderType::Vertex, "2.1.2.pbr.vs").Add(ShaderType::Fragment, "2.1.2.pbr.fs").Build("PBR Shader");
   
  #ifdef SPG_DEBUG
    pbr_shader->PrintInfo();
    GLContext::PrintVideoModes();
    GLContext::PrintGLParams();
    GLContext::PrintImplInfo();
  #endif  
    //For testing
    SPG_ASSERT(1);
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

    Window& win = m_app_context.Get<Window>("Window");

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
}