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
//#include "SpaghettiEng/Render/Mesh/MeshData.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLRenderer2.h"

#include "SpaghettiEng/Scene/Entity.h"
#include "SpaghettiEng/Scene/Registry.h"
#include "SpaghettiEng/Scene/Scene.h"
#include "SpaghettiEng/Scene/SceneManager.h"
#include "SpaghettiEng/Scene/Transform.h"

// {} []
namespace Spg
{
  SimLayer::SimLayer(ServiceLocator& service_locator, const std::string& name):
    Layer(service_locator,name),
    m_window(service_locator.Get<Window>()),
    m_renderer(service_locator.Get<GLRenderer2>()),
    m_scene_mgr(service_locator.Get<SceneManager>())
  {
    Init();
  }
    
  void SimLayer::Init()
  {
    auto& scene = m_scene_mgr.GetActiveScene();
    auto& scene_camera = m_scene_mgr.GetSceneCamera();
    scene_camera.SetAspectRatio(m_window.GetAspectRatio());

    //m_renderer.InitGpuData(scene);
    // Load mesh data to GPU
    auto& reg = scene.GetRegistry();
    auto mesh_view = reg.GetAllEntitiesWith<MeshHandle>();

    //* NOTE ent and mesh_view are raw EnTT data types - not encapsulated in registry
    for(auto ent : mesh_view)
    {
      auto& mesh_handle = reg.GetComponent<MeshHandle>(Entity{ent});
      m_renderer.InitGpuData(mesh_handle.mesh_id);
    }
  }

  void SimLayer::Shutdown()
  {
  }

  void SimLayer::Render(double delta_time) 
  {
    auto& scene = m_scene_mgr.GetActiveScene(); 
    auto& scene_camera = m_scene_mgr.GetSceneCamera();
    auto& camera_transform = m_scene_mgr.GetSceneCameraTransform();
   
    //Draw scene
    auto& reg = scene.GetRegistry();
    auto view = reg.GetAllEntitiesWith<MeshHandle, Material>();

    //* NOTE ent and view are raw EnTT data types - not encapsulated in registry
    for(auto [ent, mesh, mat] : view.each())
    {
      Entity entity{ent};
      const auto& mesh_handle = reg.GetComponent<MeshHandle>(entity);
      const auto& material = reg.GetComponent<Material>(entity);
      m_renderer.Draw(mesh_handle.mesh_id, material, 
        scene_camera, camera_transform);
    }
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
      camera_transform.RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);
  }

  void SimLayer::OnMouseScrolled(WinEvt::MouseScrolled& e)
  {
    auto& camera_camera = m_scene_mgr.GetSceneCamera();
    camera_camera.Zoom(-e.y_offset);
  }

  void SimLayer::OnMouseButtonPressed(WinEvt::MouseBtnPressed& e)
  {
  }

} 