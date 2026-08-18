#include "SpgApp/SpgApp.h"

#include <string>

#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Scene/SceneManager.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer2.h"

#include "SpgApp/SimLayer.h"
#include "SpgApp/EditorLayer.h"

using namespace std::string_literals;

namespace Spg
{
  Application* CreateApplication()
  {
    return new SpgApp("Spg App"s);
  }

  SpgApp::SpgApp(const std::string& title) :
    Spg::Application(title)
  {
    auto& resource_mgr = m_service_locator.Get<ResourceManager>();
    auto& scene_mgr = m_service_locator.Get<SceneManager>();

    scene_mgr.BuildDefaultScene(resource_mgr);
    
    m_service_locator.Register<GLRenderer2>(resource_mgr); 
    
    auto* sim_layer = new SimLayer(m_service_locator, "Sim Layer");
    auto* editor_layer = new EditorLayer(m_service_locator, "Editor Layer");

    m_layer_stack.PushLayer(sim_layer);
    m_layer_stack.PushOverlay(editor_layer);
  }

  SpgApp::~SpgApp()
  {
  }
}

int main()
{
  Spg::Application::Init();
  auto app = Spg::CreateApplication();
  app->Run();
}