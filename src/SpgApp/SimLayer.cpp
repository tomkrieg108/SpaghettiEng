#include "SpgApp/SimLayer.h"

#include "CoreLib/Core.h"

#include "SpaghettiEng/Core/ServiceLocator.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer2.h"

#include "SpaghettiEng/Core/Window.h"
#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/WindowEvents.h"
#include "SpaghettiEng/Core/ServiceLocator.h"

#include "SpaghettiEng/Render/Camera/Camera.h"
#include "SpaghettiEng/Render/Mesh/Mesh.h"
#include "SpaghettiEng/Render/Mesh/MeshData.h"
#include "SpaghettiEng/Scene/SceneManager.h"

// {} []
namespace Spg
{
  SimLayer::SimLayer(ServiceLocator& service_locator, const std::string& name):
    Layer(service_locator,name)
  {
    Init();
  }
    
  void SimLayer::Init()
  {
    m_window = m_service_locator.TryGet<Window>();
    m_renderer =  m_service_locator.TryGet<GLRenderer2>();;
    m_scene_mgr =  m_service_locator.TryGet<SceneManager>();

    SPG_ASSERT(m_window != nullptr);
    SPG_ASSERT(m_renderer != nullptr);
    SPG_ASSERT(m_scene_mgr != nullptr);

    auto& scene = m_scene_mgr->GetActiveScene();
    auto& scene_camera = m_scene_mgr->GetSceneCamera();

    scene_camera.SetAspectRatio(m_window->GetAspectRatio());
    m_renderer->InitGpuData(scene);
  }

  void SimLayer::Shutdown()
  {
  }

  void SimLayer::Render(double delta_time) 
  {
    auto& scene = m_scene_mgr->GetActiveScene(); //* Requires unordered_map lookup
    auto& scene_camera = m_scene_mgr->GetSceneCamera();
    m_renderer->Draw(scene, scene_camera);
  }

  void SimLayer::Update(double delta_time)
  {
    const float move_speed = 5.0f;
    const float t = (float)(delta_time);

    auto& scene = m_scene_mgr->GetActiveScene(); //* Requires unordered_map lookup
    auto& scene_camera = m_scene_mgr->GetSceneCamera();
    auto* input_state = m_window->GetInputState();

    if(input_state->IsKeyPressed(Key::W))
      scene_camera.MoveForward(-move_speed * t); //* note the negative value needed to move forward

    if(input_state->IsKeyPressed(Key::S))
      scene_camera.MoveForward(move_speed * t); //* note the negative value needed to move forward

    if(input_state->IsKeyPressed(Key::A))
      scene_camera.MoveRight(-move_speed * t);

    if(input_state->IsKeyPressed(Key::D))
      scene_camera.MoveRight(move_speed * t);

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
    auto& scene_camera = m_scene_mgr->GetSceneCamera();
    scene_camera.SetAspectRatio(m_window->GetAspectRatio());
  }

  void SimLayer::OnMouseMoved(WinEvt::MouseMoved& e)
  {
    auto* input_state = m_window->GetInputState();
    auto& scene_camera = m_scene_mgr->GetSceneCamera();

    if(input_state->IsMousebuttonPressed(Mouse::ButtonRight))
      scene_camera.RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);
  }

  void SimLayer::OnMouseScrolled(WinEvt::MouseScrolled& e)
  {
    m_scene_mgr->GetSceneCamera().Zoom(-e.y_offset);
  }

  void SimLayer::OnMouseButtonPressed(WinEvt::MouseBtnPressed& e)
  {
  }

} 