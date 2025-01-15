#include <iostream>

#include <SpaghettiEng/Camera/Camera2D.h>
#include <SpaghettiEng/Camera/CameraController2D.h>
#include <Geometry/Geometry.h>
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
    //BubbleContext* context = new BubbleContext();
    return new BubbleSoup("Bubble Soup"s);
  }

  BubbleLayer::BubbleLayer(AppContext& app_context, const std::string& name) : 
    Layer(app_context, name),
    m_window(app_context.Get<Window>("Window")),
    m_renderer(app_context.Get<GLRenderer>("GLRenderer")),
    m_camera_controller(app_context.Get<CameraController2D>("CameraController2D"))
  {
    m_logger = Utils::Logger::Create("Bubble Layer");
  }

  void BubbleLayer::OnAttach()
  {
    EventManager::AddHandler(this, &BubbleLayer::OnMouseMoved);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseButtonPressed);
    EventManager::AddHandler(this, &BubbleLayer::OnMouseButtonReleased);

    Camera2D& camera = m_camera_controller.GetCamera();

    glm::vec2 position = camera.GetPosition();
    glm::mat4 transform = camera.GetTransform();
    glm::mat4 view = camera.GetViewMatrix();
  }

  void BubbleLayer::OnDetach()
  {
    //Todo remove handlers
  }

  void BubbleLayer::OnMouseMoved(EventMouseMoved& e)
  {
    LOG_WARN(m_logger, "Mouse Moved: {} {}", e.delta_x, e.delta_y);
  }

  void BubbleLayer::OnMouseButtonPressed(EventMouseButtonPressed& e)
  {
    LOG_WARN(m_logger, "Mouse Btn Pressed: {}", e.btn);
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right");  
    if(e.btn == Mouse::ButtonMiddle)
      LOG_INFO(m_logger,"Middle");  
  }

  void BubbleLayer::OnMouseButtonReleased(EventMouseButtonReleased& e)
  {
    LOG_WARN(m_logger, "Mouse Btn Released: {}", e.btn);
    if(e.btn == Mouse::ButtonLeft)
      LOG_INFO(m_logger,"Left");
    if(e.btn == Mouse::ButtonRight)
      LOG_INFO(m_logger,"Right");  
    if(e.btn == Mouse::ButtonMiddle)
      LOG_INFO(m_logger,"Middle");  
  }

  void BubbleLayer::OnImGuiRender()
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
    
    auto camera = CreateRef<Camera2D>();
    auto Camera_controller = CreateRef<CameraController2D>(*camera);

    m_app_context.Set("CameraController2D", Camera_controller);

    BubbleLayer* layer = new BubbleLayer(m_app_context, std::string("Bubble Layer"));
    PushLayer(layer);

    //Todo - compile error
    //SPG_WARN("Camera Pos: {}", position);
    //SPG_WARN("Camera Transform: {}", transform);
    //SPG_WARN("Camera View: {}", view);
  }

  BubbleSoup::~BubbleSoup()
  {

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
  Spg::Application::SystemInit();
#ifdef SPG_LIB_LINK_CHECK 
  Utils::LibCheck();
  Geom::GeomLibHello();
  Spg::AppPrintHello();
#endif

  auto app = Spg::CreateApplication();
  app->Run();
  delete app; //Doesn't get called in release mode!
  return 0;
}