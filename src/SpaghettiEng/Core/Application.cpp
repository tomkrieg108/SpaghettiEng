
#include <filesystem>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

//only for ImGui::ShowDemoWindow();
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Events/EventManager.h"
#include "ImGuiLayer/ImGuiLayer.h"
#include "OpenGL32/GL32Shader.h"

#include "Geometry/Geometry.h"
#include "Application.h"

//external libs test
//should be available because they were linked as public in Geom lib, which links to eng_lib
//only for void EngLibHello()
  #include <cxxopts.hpp>
  #include <fmt/format.h>
  #include <nlohmann/json.hpp>
  #include <stb_image.h>
  #include <glm/glm.hpp>
  #include <spdlog/spdlog.h>
//---------------------------------------


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

  Application::Application(const std::string& title) : m_app_title(title) 
  {
    SPG_ASSERT(s_instance == nullptr);
    s_instance = this;
    Utils::Logger::Initialise();
    m_log = Utils::Logger::Create("ENG");
    EventManager::Initialise(); 
    m_window = Window::Create(title);
    ImGuiLayer::Initialise(*m_window);
    SetEventHandlers();
    SetAssetsPath();
    
    //Build test shaders
    GLShaderBuilder shader_builder;
    auto pbr_shader = shader_builder.Add(ShaderType::Vertex, "2.1.2.pbr.vs").Add(ShaderType::Fragment, "2.1.2.pbr.fs").Build("PBR Shader");
    pbr_shader->PrintInfo();

    // auto pbr_shader = shader_builder.Add(ShaderType::Vertex, "2.2.basic_lighting.vs").Add(ShaderType::Fragment, "2.2.basic_lighting.fs").Build("Basic Lighting Shader");
    // pbr_shader->PrintInfo();
    
    SPG_ASSERT(1);
  }

  Application::~Application()
  {
    ImGuiLayer::Shutdown();
    m_window->Shutdown();
  }

  Application& Application::Get()
  {
    SPG_ASSERT(s_instance != nullptr);
    return *s_instance;
  }

  Window& Application::GetWindow()
  {
    SPG_ASSERT(m_window != nullptr);
    return *m_window;
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
    // Step 1: Get the current file path
    fs::path this_file = fs::absolute(fs::path{__FILE__});
    //SPG_INFO("This file path: {}", this_file.string());

    // Step 2: Resolve the assets path
    fs::path assets_path = this_file.parent_path() / fs::path{"../../../Assets"};
    //SPG_INFO("Constructed assets path: {}", assets_path.string());

    // Step 3: Convert to an absolute path for safety
    assets_path = fs::absolute(assets_path);
    //SPG_INFO("Absolute assets path: {}", assets_path.string());

    // Step 4: Check if the directory exists
    if (!fs::exists(assets_path)) {
        SPG_ERROR("Assets path does not exist: {}", assets_path.string());
        return;
    }

    // Step 5: Check if it's a directory
    if (!fs::is_directory(assets_path)) {
        SPG_ERROR("Assets path is not a directory: {}", assets_path.string());
        return;
    }

    // Step 6: Set the current working directory
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

    while(m_running)
    {
      auto now = glfwGetTime(); //in seconds
      delta_time = now - last_time;
      last_time = now;

      m_window->ClearBuffers();
      m_window->PollEvents();
      
      if(!m_window->IsMinimised())
      {
        for (Layer* layer : m_layer_stack)
		      layer->OnUpdate(delta_time);

        ImGuiLayer::PreRender();
        ImGuiAppRender();
        for (Layer* layer : m_layer_stack)
		      layer->OnImGuiRender();
        ImGui::ShowDemoWindow();
        ImGuiLayer::PostRender();
      }
      m_window->SwapBuffers();
      EventManager::DispatchQueuedEvents();
    }
  }

  void Application::ImGuiAppRender()
  {
    auto& params = m_window->GetParams();
   
    ImGui::Begin("App");
    if (ImGui::CollapsingHeader("Window: Size,Framerate"))
    {
      ImGui::Text("Width,Height %d %d : ", params.width, params.height);
      ImGui::Text("Buff Width, Buff Height %d %d : ", params.buffer_width, params.buffer_height);
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().
      Framerate);
      ImGui::Checkbox("VSync Enable: ", &(params.vsync_enabled));
      m_window->SetVSyncEnabled(params.vsync_enabled);
    }
    ImGui::End();
  }

  void EngLibHello()
  {
    std::cout << "\nHello from Eng Lib\n";
    #ifdef SPG_DEBUG
      std::cout << "ENG: SPG_DEGUG defined\n";
    #elif defined(SPG_RELEASE)
      std::cout << "ENG: SPG_RELEASE defined\n";
    #endif

    #ifdef _WIN32
      std::cout << "ENG: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "ENG: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "ENG: x86_64 architecture\n";
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

    std::cout << "double pi      " << std::numbers::pi << std::endl;
    std::cout << "####################################################\n\n";
  }
}