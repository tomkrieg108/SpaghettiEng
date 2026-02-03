#include "GeomApp.h"
#include "DefaultLayer.h"

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
    #ifdef _WIN32
      auto text_renderer = CreateRef<GLTextRenderer>(*camera);
    #endif

    m_app_context.Set("Camera2D", camera);
    m_app_context.Set("CameraController2D", controller);
    #ifdef _WIN32
      m_app_context.Set("GLTextRenderer", text_renderer);
    #endif

    DefaultLayer* layer = new DefaultLayer(m_app_context, std::string("Default Layer"));
    PushLayer(layer);
  }

  GeomApp::~GeomApp()
  {
  }

  void HelloGeomApp()
  {
    SPG_WARN("Hello From Geom App");
    
    #ifdef SPG_DEBUG
      SPG_TRACE("APP: SPG_DEBUG defined");
    #elif defined(SPG_RELEASE)
        SPG_TRACE("APP: SPG_RELEASE defined");
    #endif  

    #ifdef _WIN32
      SPG_TRACE("APP: WIN platform");
    #elif defined(__linux__)
      SPG_TRACE("APP: LINUX platform");
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
      SPG_TRACE("APP: x86_64 architecture");
    #endif

    SPG_WARN("External libs linked into App exe:\n");
    SPG_TRACE("JSON: {}.{}.{}", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH );
    SPG_TRACE( "FMT: {} ", FMT_VERSION);
    SPG_TRACE("CXXOPTS: {}.{}.{}",CXXOPTS__VERSION_MAJOR,CXXOPTS__VERSION_MAJOR, CXXOPTS__VERSION_PATCH);
    SPG_TRACE("SPDLOG: {},{}.{}",SPDLOG_VER_MAJOR,SPDLOG_VER_MINOR,SPDLOG_VER_PATCH);
    #ifdef _WIN32
      SPG_TRACE( "FREETYPE: {}.{}.{}", FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
    #endif
    SPG_TRACE("Entt {},{},{}", ENTT_VERSION_MAJOR, ENTT_VERSION_MINOR, ENTT_VERSION_PATCH );
    SPG_WARN( "####################################################");
  }
}

int main()
{
  Spg::Application::SystemInit();
#ifdef SPG_LIB_LINK_CHECK 
  Utils::HelloUtilsLib();
  MathX::HelloMathLib();
  Phys::HelloPhysLib();
  Geom::HelloGeomLib();
  Spg::HelloGeomApp();
#endif

  auto app = Spg::CreateApplication();
  app->Run();
  delete app; //Doesn't get called in release mode!
}