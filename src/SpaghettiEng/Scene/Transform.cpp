#include "SpaghettiEng/Scene/Transform.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

//  []
namespace Spg
{
  inline namespace Transform_v1
  {
    void Translate(Transform& transform,  glm::vec3& move)
    {
      transform.matrix = glm::translate(transform.matrix, move);
    }

    void Scale(Transform& transform, const glm::vec3& scale)
    {
      transform.matrix = glm::scale(transform.matrix, scale);
    }

    void Rotate(Transform& transform, float angle_deg, const glm::vec3& axis)
    {
      transform.matrix = glm::rotate(transform.matrix, glm::radians(angle_deg),
              axis);
    }

  } 

} 
