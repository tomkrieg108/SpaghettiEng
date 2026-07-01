#include "GeomApp.h"
#include "DefaultLayer.h"

#include "MathLib/MathLib.h"

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
    m_service_locator.Register<GLRenderer>();
    m_service_locator.Register<Camera2D>();

    auto& window = m_service_locator.Get<Window>();
    auto& camera = m_service_locator.Get<Camera2D>();

    m_service_locator.Register<CameraController2D>(camera);
    #ifdef _WIN32
      m_service_locator.Register<GLTextRenderer>(camera);
    #endif

    glm::vec3 cam_pos = glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 look_pos = glm::vec3(0.0f,0.0f,0.0f);
    camera.SetPosition(cam_pos);
    camera.LookAt(look_pos);

    DefaultLayer* layer = new DefaultLayer(m_service_locator, std::string("Default Layer"));
    PushLayer(layer);
  }

  GeomApp::~GeomApp()
  {
  }
}

int main()
{
  Spg::Application::SystemInit();
#ifdef SPG_LIB_LINK_CHECK 
 Spg::Application::PrintPlatformInfo();
 Spg::Application::PrintExternalLibInfo();
#endif
  auto app = Spg::CreateApplication();
  app->Run();
}