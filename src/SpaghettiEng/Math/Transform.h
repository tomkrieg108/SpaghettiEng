#pragma once

#include <type_traits> //for V2 only

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>


// {} []

namespace Spg
{
  inline namespace Transform_V2
  {

    struct Transform
    { 
      glm::vec3 position = {0,0,0};
      glm::vec3 scale = {1,1,1};
      glm::quat orientation = glm::quat(); //(w,x,y,z), with w the real part

      void SetOrientation(const glm::vec3& euler_angles_xyz_deg);
      void Translate(const glm::vec3& delta_pos);
      void Turn(float pitch, float yaw);
      void LookAt(const glm::vec3& look_pos);
      glm::mat4 ToMat4() const;
      glm::mat4 Inverse();

      void MoveForward(float amount)
      {
        Translate(glm::vec3(0, 0, amount));
      }

      void MoveRight(float amount)
      {
        Translate(glm::vec3(amount, 0, 0));
      }

      void SetPosition(const glm::vec3& position)
      {
        this->position = position;
      }
      

    }; 

  }
  

  //==============================================================

  namespace Transform_v1
  {
    struct Transform
    {
      glm::mat4 matrix = glm::mat4(1.0f);
     
      glm::vec3 Position() const;
      glm::vec3 Front() const;
      glm::vec3 Up() const;
      glm::vec3 Right() const;
      glm::mat4 Inverse() const;

      void InvertXYAxes();
      void SetPosition(const glm::vec3& position);
      void LookAt(const glm::vec3& look_pos);
      void LookAt(const glm::vec3& look_pos, const glm::vec3& up);
      void MoveForward(float amount);
      void MoveRight(float amount);
      void MoveVertically(float amount);
      void Turn(float delta_yaw, float delta_pitch);
      void RotateWorld(float amount_x, float amount_y);

    };

    void Translate(Transform& transform,  glm::vec3& vec);
    void Scale(Transform& transform, const glm::vec3& scale);
    void Rotate(Transform& transform, float angle_deg, const glm::vec3& axis);
  }

  //static_assert(std::is_trivially_copyable_v<Transform>,"Transform class not trivially copiable");
}