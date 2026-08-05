#pragma once

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

      glm::vec3 position = glm::vec3(0.0f);
      glm::vec3 scale = glm::vec3(1.0f);
      glm::vec3 euler_angles = glm::vec3(0.0f);
    };

    void Translate(Transform& transform,  glm::vec3& vec);
    void Scale(Transform& transform, const glm::vec3& scale);
    void Rotate(Transform& transform, float angle_deg, const glm::vec3& axis);
  }
  



} 




