
#include <filesystem>

#include <glad/gl.h>    //need to include before <GLFW/glfw3.h>
#include <GLFW/glfw3.h> //for glfwGetTime(), GLFW_KEY_ESCAPE

#include "Events/EventManager.h"
#include "ImGuiContext/ImGuiContext.h"
#include "OpenGL32/GL32Context.h"
#include "OpenGL32/GL32Shader.h"
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

  void Application::InitialiseAppContext(const std::string& title)
  {
    m_app_context.window = Window::Create(title);
    m_app_context.renderer = GLRenderer::Create();
    m_app_context.title = title;
    m_app_context.log = Utils::Logger::Create("ENG");
    m_app_context.running = true;   
  }

  Application::Application(const std::string& title) 
  {
    SPG_ASSERT(s_instance == nullptr);
    s_instance = this;
    
    Utils::Logger::Initialise();
    //m_log = Utils::Logger::Create("ENG");
    EventManager::Initialise(); //todo - does nothing!
    //m_window = Window::Create(title);
    //m_renderer = GLRenderer::Create();
    SetEventHandlers();
    SetAssetsPath();
    

    InitialiseAppContext(title);
    ImGuiContext::Initialise(*m_app_context.window);
    
    //Test shader
    GLShaderBuilder shader_builder;
    auto pbr_shader = shader_builder.Add(ShaderType::Vertex, "2.1.2.pbr.vs").Add(ShaderType::Fragment, "2.1.2.pbr.fs").Build("PBR Shader");
   
    AppLayer* app_layer = new AppLayer(m_app_context, "App Layer");
    m_layer_stack.PushOverlay(app_layer);
 
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
    ImGuiContext::Shutdown();
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
    m_app_context.running = false;
    e.handled = true;   
  }

  void Application::OnKeyPressed(EventKeyPressed& e)
  {
    SPG_WARN("Key pressed ** {} ", e.key);
    if(e.key == GLFW_KEY_ESCAPE)
    {
      m_app_context.running = false;
      e.handled = true;   
    }
  }

  void Application::Run()
  {
    SPG_WARN("App loop starting");
    auto delta_time = 0.0;
    auto last_time = glfwGetTime();

    while(m_app_context.running)
    {
      auto now = glfwGetTime(); //in seconds
      delta_time = now - last_time;
      last_time = now;

      m_app_context.window->Clear();
     
      if(!m_app_context.window->IsMinimised())
      {
        for (Layer* layer : m_layer_stack)
		      layer->OnUpdate(delta_time);

        ImGuiContext::PreRender();
        for (Layer* layer : m_layer_stack)
		      layer->OnImGuiRender();
        ImGuiContext::PostRender();
      }

      m_app_context.window->OnUpdate();

      EventManager::DispatchQueuedEvents();
    }
  }
}