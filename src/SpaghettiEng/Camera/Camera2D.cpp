
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

  glm::vec2 Camera2D::GetPosition()
  {
    glm::vec2 pos = (glm::vec2)m_transform[3];
    return pos;
  }

}