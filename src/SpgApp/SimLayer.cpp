#include "SpgApp/SimLayer.h"

#include "CoreLib/Core.h"

#include "SpaghettiEng/Core/ServiceLocator.h"

#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/ServiceLocator.h"

#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/Material.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer.h"

#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/Scene.h"
#include "SpaghettiEng/Scene/SceneManager.h"
//#include "SpaghettiEng/Scene/Transform.h"
#include "SpaghettiEng/Math/Transform.h"

// {} []
namespace Spg
{
  SimLayer::SimLayer(ServiceLocator& service_locator, const std::string& name):
    Layer(service_locator,name),
    m_window(service_locator.Get<Window>()),
    m_renderer(service_locator.Get<GLRenderer>()),
    m_scene_mgr(service_locator.Get<SceneManager>())
  {
    Init();
  }
    
  void SimLayer::Init()
  {
  }

  void SimLayer::Shutdown()
  {
  }

  void SimLayer::Render(double delta_time) 
  {
    m_renderer.DrawActiveScene();
  }

  void SimLayer::Update(double delta_time)
  {
    const float move_speed = 5.0f;
    const float t = (float)(delta_time);

    auto& scene = m_scene_mgr.GetActiveScene(); 
    auto& camera_transform = m_scene_mgr.GetSceneCameraTransform();
    auto* input_state = m_window.GetInputState();

    if(input_state->IsKeyPressed(Key::W))
      camera_transform.MoveForward(-move_speed * t); // negative value needed to move forward

    if(input_state->IsKeyPressed(Key::S))
      camera_transform.MoveForward(move_speed * t);

    if(input_state->IsKeyPressed(Key::A))
      camera_transform.MoveRight(-move_speed * t);

    if(input_state->IsKeyPressed(Key::D))
      camera_transform.MoveRight(move_speed * t);
  }

  void SimLayer::OnEvent(WinEvt::Event& event)
  {
    switch(event.type)
    {
      case WinEvt::EventType::WindowResize: 
        OnWindowResize(static_cast<WinEvt::WindowResize&>(event)); break;
      case WinEvt::EventType::MouseMoved: 
        OnMouseMoved(static_cast<WinEvt::MouseMoved&>(event)); break; 
      case WinEvt::EventType::MouseScrolled: 
        OnMouseScrolled(static_cast<WinEvt::MouseScrolled&>(event)); break; 
      case WinEvt::EventType::MouseBtnPressed: 
        OnMouseButtonPressed(static_cast<WinEvt::MouseBtnPressed&>(event)); break; 
    } 
  }

  void SimLayer::OnWindowResize(WinEvt::WindowResize& e)
  {
    auto& scene_camera = m_scene_mgr.GetSceneCamera();
    scene_camera.SetAspectRatio(m_window.GetAspectRatio());
  }

  void SimLayer::OnMouseMoved(WinEvt::MouseMoved& e)
  {
    auto* input_state = m_window.GetInputState();
    auto& camera_transform = m_scene_mgr.GetSceneCameraTransform();

    if(input_state->IsMousebuttonPressed(Mouse::ButtonRight))
      camera_transform.Turn(e.delta_x * 0.001f, e.delta_y * 0.05f);
  }

  void SimLayer::OnMouseScrolled(WinEvt::MouseScrolled& e)
  {
    auto& camera_transform = m_scene_mgr.GetSceneCameraTransform();
    camera_transform.MoveForward(-e.y_offset);
    // auto& camera_camera = m_scene_mgr.GetSceneCamera();
    // camera_camera.Zoom(-e.y_offset);
  }

  void SimLayer::OnMouseButtonPressed(WinEvt::MouseBtnPressed& e)
  {
  }

} 