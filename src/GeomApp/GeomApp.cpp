
#include "GeomApp/GeomApp.h"

#include "GeomApp/DefaultLayer.h"
#include "GeomApp/UILayer.h"

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

    //Todo - initializing the renderer service requires shader_cache loaded - not ideal - get rid of this later!
    auto& resource_manager = m_service_locator.Get<ResourceManager>();
    auto& shader_cache = resource_manager.GetResourceCache<GLShader>();
    m_service_locator.Get<GLRenderer>().SetShaderCache(shader_cache); 

    //Todo - Are Camera2D,CameraController2D really services?
    m_service_locator.Register<Camera2D>();
    auto& camera = m_service_locator.Get<Camera2D>();

    //Todo - Resistering a service shouldn't depend on other services already being registered
    m_service_locator.Register<CameraController2D>(camera);
  #ifdef _WIN32
    // Todo GLTextRenderer shouldn't depend on camera - camera should be arg to methods
    m_service_locator.Register<GLTextRenderer>(camera);
    m_service_locator.Get<GLTextRenderer>().Init(resource_manager.GetAssetsPath(), shader_cache); 
  #endif

    glm::vec3 cam_pos = glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 look_pos = glm::vec3(0.0f,0.0f,0.0f);
    camera.SetPosition(cam_pos);
    camera.LookAt(look_pos);

    DefaultLayer* default_layer = new DefaultLayer(m_service_locator, std::string("Default Layer"));
    SPG_ASSERT(default_layer != nullptr);
    UILayer* ui_layer = new UILayer(default_layer, m_service_locator, "UI Layer");

    PushLayer(default_layer);
    m_layer_stack.PushOverlay(ui_layer);
    //PushLayer(ui_layer);
  }

  GeomApp::~GeomApp()
  {
  }
}

int main()
{
  Spg::Application::Init();
  auto app = Spg::CreateApplication();
  app->Run();
}