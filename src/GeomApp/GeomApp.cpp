#include "GeomApp.h"
#include "DefaultLayer.h"

//external libs - for testing out only
//should be available because they were linked as public in Geom/Spg lib and propagated here
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include "ft2build.h"
#include <freetype/freetype.h>

namespace Spg
{
  
  Application* CreateApplication()
  {
    return new GeomApp("Geom App"s);
  }

  //---------------------------------------------------------------------------------

  GeomApp::GeomApp(const std::string& title) : 
    Application(title)
  {
    Window& window = m_app_context.Get<Window>("Window");
    glm::vec3 cam_pos = glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 look_pos = glm::vec3(0.0f,0.0f,0.0f);
    auto camera = CreateRef<Camera2D>();
    camera->SetPosition(cam_pos);
    camera->LookAt(look_pos);
    auto controller = CreateRef<CameraController2D>(*camera);
    auto text_renderer = CreateRef<GLTextRenderer>(*camera);

    m_app_context.Set("Camera2D", camera);
    m_app_context.Set("CameraController2D", controller);
    m_app_context.Set("GLTextRenderer", text_renderer);

    DefaultLayer* layer = new DefaultLayer(m_app_context, std::string("Default Layer"));
    PushLayer(layer);
    float log2 = glm::log2(1000.0f);
    float sqr_root = glm::sqrt(1000.0f);

    SPG_WARN("log2 {}", log2);
    SPG_WARN("log2_sq {}", log2*log2);
    SPG_WARN("sqr_root {}", sqr_root);
  }

  GeomApp::~GeomApp()
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

    SPG_WARN( "FREETYPE: {}.{}.{}", FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);

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
}