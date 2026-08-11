#pragma once

#include <type_traits>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

// {} []

namespace Spg
{
  inline namespace Transform_v1
  {
    struct Transform
    {
      glm::mat4 matrix = glm::mat4(1.0f);
     
      glm::vec3 Position();
      glm::vec3 Front();
      glm::vec3 Up();
      glm::vec3 Right();
      void InvertXYAxes();
      void SetPosition(const glm::vec3& position);
      void LookAt(const glm::vec3& look_pos);
      void LookAt(const glm::vec3& look_pos, const glm::vec3& up);
      void MoveForward(float amount);
      void MoveRight(float amount);
      void MoveVertically(float amount);
      void Turn(float delta_yaw, float delta_pitch);
      void RotateLocal(float delta_yaw, float delta_pitch);
      void RotateWorld(float amount_x, float amount_y);

    };

    void Translate(Transform& transform,  glm::vec3& vec);
    void Scale(Transform& transform, const glm::vec3& scale);
    void Rotate(Transform& transform, float angle_deg, const glm::vec3& axis);
  }

  static_assert(std::is_trivially_copyable_v<Transform>,"Transform class not trivially copiable");


  namespace Transform_V2
  {
    struct Transform
    {
      glm::vec3 position = glm::vec3(0.0f);
      glm::vec3 scale = glm::vec3(1.0f);
      //glm::vec3 euler_angles = glm::vec3(0.0f);
      //glm::quat orientation;

    };


  }
  



} 




