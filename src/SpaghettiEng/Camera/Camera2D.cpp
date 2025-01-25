
#include <glm/glm.hpp>
#include "Core/Window.h"
#include "Camera2D.h"

namespace Spg
{
  Camera2D::Camera2D()
  {
    glm::vec4 v4_pos = glm::vec4(0, 0, -0.5f, 1.0f);
    m_transform[3] = v4_pos;
  }

  glm::mat4 Camera2D::GetProjMatrix() const
  {
    //near and far are -1,1 if not specified
    return glm::ortho(m_params.left, m_params.right, m_params.bottom, m_params.top);
  }

  glm::mat4 Camera2D::GetViewMatrix() const
  {
    return glm::inverse(m_transform);
  }  

  glm::mat4& Camera2D::GetTransform() 
  { 
    return m_transform; 
  }

  Camera2D::Params Camera2D::GetParams() const
  {
    return m_params;    
  }

  void Camera2D::SetParams(const Camera2D::Params& params)
  {
    m_params = params;
  }

  glm::vec2 Camera2D::GetPosition() const
  {
    glm::vec2 pos = (glm::vec2)m_transform[3];
    return pos;
  }

  void Camera2D::SetPosition(glm::vec3& position)
  {
      m_transform[3] = glm::vec4{ position, 1.0f };
  }

  glm::vec3 Camera2D::Front() const
  {
    glm::vec3 z = (glm::vec3)m_transform[2];
    return -z;  //camera looks in -ve z dir
  }

  glm::vec3 Camera2D::Right() const
  {
    return (glm::vec3)m_transform[0]; //local x
  }

  glm::vec3 Camera2D::Up() const
  {
    return (glm::vec3)m_transform[1]; //local y
  }
  
  void Camera2D::LookAt(glm::vec3& look_pos)
  {
    glm::vec3 pos = (glm::vec3)m_transform[3];
    glm::vec3 z = -glm::normalize(look_pos - pos); // negative front
    glm::vec3 up = glm::vec3(0, 1, 0);
    // Check if z is parallel to the global up vector
    if (glm::abs(glm::dot(z, up)) > 0.99f) {
        // Choose a different "up" vector to avoid degeneracy
        up = glm::vec3(1, 0, 0); // Global X-axis as a fallback
    }
    glm::vec3 x = glm::normalize(glm::cross(up,z));
    glm::vec3 y = glm::normalize(glm::cross(z, x));
    m_transform[0] = glm::vec4{ x,0.0f };
    m_transform[1] = glm::vec4{ y,0.0f };
    m_transform[2] = glm::vec4{ z,0.0f };
  }

  void Camera2D::LookAt(glm::vec3& look_pos, glm::vec3& up)
  {
    glm::vec3 pos = glm::vec3(m_transform[3]);
    glm::vec3 z = -glm::normalize(look_pos - pos); // Negative front
    glm::vec3 x = glm::normalize(glm::cross(up, z));
    glm::vec3 y = glm::normalize(glm::cross(z, x));
    m_transform[0] = glm::vec4{ x, 0.0f };
    m_transform[1] = glm::vec4{ y, 0.0f };
    m_transform[2] = glm::vec4{ z, 0.0f };
  }

}