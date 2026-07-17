# include "SpaghettiEng/Scene/SceneCameraController.h"

#include "SpaghettiEng/Core/InputState.h"
#include "SpaghettiEng/Core/KeyCodes.h"
#include "SpaghettiEng/Core/MouseCodes.h"
#include "SpaghettiEng/Scene/Components.h"


namespace Spg
{
  /*
    {
    }
  */

  static float s_move_speed = 5.0f;

  void SceneCameraController::OnUpdate(Transform& transform, InputState& input_state, double delta_time)
  {
    float dt = static_cast<float>(delta_time);

    if(input_state.IsKeyPressed(Key::W))
      MoveForward(transform, dt * s_move_speed);
    if(input_state.IsKeyPressed(Key::S))
      MoveForward(transform, -dt * s_move_speed);
    if(input_state.IsKeyPressed(Key::D))
      MoveRight(transform, dt * s_move_speed);
    if(input_state.IsKeyPressed(Key::A))
      MoveRight(transform, -dt * s_move_speed);  
  }

  void SceneCameraController::MoveForward(Transform& transform, float amount)
  {
    glm::vec3 direction = transform.LocalForward();
    transform.Translate(direction*amount);    
  }

  void SceneCameraController::MoveRight(Transform& transform, float amount) 
  {
    transform.Translate(glm::vec3(1,0,0)*amount);  
  }

  void SceneCameraController::MoveVertically(Transform& transform, float amount) 
  {
    transform.Translate(glm::vec3(0,1,0)*amount);  
  }

  void SceneCameraController::SetEnabled(bool enabled) 
  {
    m_enabled = enabled;
  }

  bool SceneCameraController::IsEnabled() const 
  {
    return m_enabled;
  }

}