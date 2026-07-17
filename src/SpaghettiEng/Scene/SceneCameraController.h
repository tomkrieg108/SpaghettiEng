#pragma once

namespace Spg
{
  class InputState;
  class Transform;

  class SceneCameraController
  {
    public:

      SceneCameraController() = default;
      ~SceneCameraController() = default;

      void OnUpdate(Transform& transform, InputState& input_state, double delta_time);
      void SetEnabled(bool enabled);
      bool IsEnabled() const;

    private:
      void MoveForward(Transform& transform, float amount);
      void MoveRight(Transform& transform, float amount);
      void MoveVertically(Transform&, float amount);
    
    private:  
      bool m_enabled = true;
  };
}
