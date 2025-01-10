#include <iostream>

#include "Geometry/Geometry.h"
#include "BubbleSoup.h"

//external libs
//should be available because they were linked as public in Geom lib and propagated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
//--------------------------------------------

namespace Spg
{
  namespace fs = std::filesystem;

  Application* CreateApplication()
  {
    return new BubbleSoup("Bubble Soup"s);
  }

  DefaultLayer::DefaultLayer(const AppContext& app_context, const std::string& name) : 
    Layer(app_context, name)  
  {
    m_logger = Utils::Logger::Create("Default Layer");
  }

  void DefaultLayer::OnAttach()
  {
    EventManager::AddHandler(this, &DefaultLayer::OnMouseMoved);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseButtonPressed);
    EventManager::AddHandler(this, &DefaultLayer::OnMouseButtonReleased);
  }

  void DefaultLayer::OnDetach()
  {
  }

  void DefaultLayer::OnMouseMoved(EventMouseMoved& e)
  {
    LOG_WARN(m_logger, "Mouse Moved: {} {}", e.delta_x, e.delta_y);
  }

  void DefaultLayer::OnMouseButtonPressed(EventMouseButtonPressed& e)
  {
    LOG_WARN(m_logger, "Mouse Btn Pressed: {}", e.btn);
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right");  
    if(e.btn == Mouse::ButtonMiddle)
      LOG_INFO(m_logger,"Middle");  
  }

  void DefaultLayer::OnMouseButtonReleased(EventMouseButtonReleased& e)
  {
    LOG_WARN(m_logger, "Mouse Btn Released: {}", e.btn);
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right");  
    if(e.btn == Mouse::ButtonMiddle)
      LOG_INFO(m_logger,"Middle");  
  }

  void DefaultLayer::OnImGuiRender()
  {
    ImGui::Begin(m_name.c_str());
    if (ImGui::CollapsingHeader("Gui For this layer"))
    {
      ImGui::Text("Data and controls for this layer goes here");
    }
    ImGui::End();
  }

  //---------------------------------------------------------------------------------

  BubbleSoup::BubbleSoup(const std::string& title) : 
    Application(title) 
  {
    m_default_layer = new Spg::DefaultLayer(this->m_app_context, std::string("Default Layer"));
    PushLayer(m_default_layer);

    glm::vec2 position = m_camera.GetPosition();
    glm::mat4 transform = m_camera.GetTransform();
    glm::mat4 view = m_camera.GetViewMatrix();

    //Todo - compile error
    //SPG_WARN("Camera Pos: {}", position);
    //SPG_WARN("Camera Transform: {}", transform);
    //SPG_WARN("Camera View: {}", view);
  }

  BubbleSoup::~BubbleSoup()
  {
    PopLayer(m_default_layer);
  }

  void AppPrintHello()
  {
    std::cout << "Hello From App\n";
    
    #ifdef SPG_DEBUG
      std::cout << "APP: SPG_DEBUG defined\n";
    #elif defined(SPG_RELEASE)
        std::cout << "APP: SPG_RELEASE defined\n";
    #endif  

    #ifdef _WIN32
      std::cout << "APP: WIN platform\n";
    #elif defined(__linux__)
      std::cout << "APP: LINUX platform\n";
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      std::cout << "APP: x86_64 architecture\n";
    #endif

    std::cout << "External libs linked into App exe:\n";
    std::cout << "JSON:" << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << "\n";
    std::cout << "FMT:" << FMT_VERSION << "\n";
    std::cout << "CXXOPTS:" << CXXOPTS__VERSION_MAJOR << "." << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << "\n";
    std::cout << "SPDLOG....:" << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH << "\n";
    std::cout << "\n";

    std::cout << "####################################################\n\n";
  }
}

int main()
{
#ifdef SPG_LIB_LINK_CHECK 
  Utils::LibCheck();
  Geom::GeomLibHello();
  Spg::AppPrintHello();
#endif

  auto app = Spg::CreateApplication();
  app->Run();
  delete app; //Doesn't call this in release mode!
  return 0;
}