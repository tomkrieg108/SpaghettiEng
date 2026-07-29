#include "SpgApp/SpgApp.h"

#include <string>

#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/Render/Renderer.h"
#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Scene/SceneManager.h"
#include "SpaghettiEng/Scene/SceneVisuals.h"

#include "SpgApp/EditorLayer.h"

//Test only
#include "SpaghettiEng/Scene/Registry.h"


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
    m_service_locator.Register<Renderer>();
    m_service_locator.Register<SceneVisuals>();

    EditorLayer* editor_layer = new EditorLayer(m_service_locator, "Editor Layer");
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